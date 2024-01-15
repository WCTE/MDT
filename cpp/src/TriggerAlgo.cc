#include "TriggerAlgo.h"
#include "Configuration.h"
#include "HitTube.h"

const double TriggerAlgo::fLongTime = 40E9; // ns = 40s. loooong time for SN simulations

TriggerAlgo::TriggerAlgo(const string &pmtname) :
fTriggerTimeForFailure( 100. ),
fNDigitsWindow( 200. ),
fNDigitsStepSize( 5. ),
fNDigitsThreshold( 25 ),
fTriggerType ( TriggerType::eNDigits ),
fPMTType( pmtname )
{
    fPreTriggerWindow[TriggerType::eNDigits] = -400.;
    fPreTriggerWindow[TriggerType::eFailure] = -400.;
    fPreTriggerWindow[TriggerType::eNoTrig]  = -fLongTime;
    
    fPostTriggerWindow[TriggerType::eNDigits] = 950.;
    fPostTriggerWindow[TriggerType::eFailure] = 100000.;
    fPostTriggerWindow[TriggerType::eNoTrig]  = fLongTime;

    map<string, string> s;
    s["NDigitsWindow"] = "NDigitsWindow";
    s["NDigitsStepSize"] = "NDigitsStepSize";
    s["NDigitsThreshold"] = "NDigitsThreshold";
    s["FailureTime"] = "FailureTime";
    s["NDigitsPreTriggerWindow"] = "NDigitsPreTriggerWindow";
    s["NDigitsPostTriggerWindow"] = "NDigitsPostTriggerWindow";
    s["FailurePreTriggerWindow"] = "FailurePreTriggerWindow";
    s["FailurePostTriggerWindow"] = "FailurePostTriggerWindow";
    s["TriggerType"] = "TriggerType";

    if( fPMTType!="" )
    {
        map<string, string>::iterator i;
        for(i=s.begin(); i!=s.end(); i++)
        {
            i->second += "_" + fPMTType;
        }
    }

    string sTriggerType;

    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>(s["NDigitsWindow"], fNDigitsWindow);
    Conf->GetValue<float>(s["NDigitsStepSize"], fNDigitsStepSize);
    Conf->GetValue<int>(s["NDigitsThreshold"], fNDigitsThreshold );
    Conf->GetValue<float>(s["FailureTime"], fTriggerTimeForFailure);
    Conf->GetValue<float>(s["NDigitsPreTriggerWindow"], fPreTriggerWindow[TriggerType::eNDigits]);
    Conf->GetValue<float>(s["NDigitsPostTriggerWindow"], fPostTriggerWindow[TriggerType::eNDigits]);
    Conf->GetValue<float>(s["FailurePreTriggerWindow"], fPreTriggerWindow[TriggerType::eFailure]);
    Conf->GetValue<float>(s["FailurePostTriggerWindow"], fPostTriggerWindow[TriggerType::eFailure]);
    Conf->GetValue<string>(s["TriggerType"], sTriggerType);

    if ( sTriggerType=="NDigits" )
    {
        fTriggerType = TriggerType::eNDigits;
        cout<<" Use default NDigits trigger for "<<fPMTType<<endl;
    }
    else if ( sTriggerType=="NoTrig" )
    {
        fTriggerType = TriggerType::eNoTrig;
        cout<<" Use NoTrig trigger for "<<fPMTType<<endl;
    }
    else 
    {
        fTriggerType = TriggerType::eNDigits;
        cout<<" Unknown trigger type! Use default NDigits trigger for "<<fPMTType<<endl;
    }
}

void TriggerAlgo::DoTrigger(HitTubeCollection *hc, TriggerInfo* ti)
{
    if ( fTriggerType == TriggerType::eNDigits )
    {
        NDigits(hc, ti);
    }
    else
    {
        NoTrig(hc, ti);
    }
}

void TriggerAlgo::NoTrig(HitTubeCollection *hc, TriggerInfo* ti)
{
    float trigTime = 0.;
    int nHits = hc->GetTotalNumOfDigiHits();
    float trigTimeLow = trigTime + fPreTriggerWindow[TriggerType::eNoTrig];
    float trigTimeUp  = trigTime + fPostTriggerWindow[TriggerType::eNoTrig];
    ti->AddTrigger( trigTime,
                    trigTimeLow,
                    trigTimeUp,
                    nHits, 
                    (int)TriggerType::eNoTrig);

    cout<<" Found trigger at: " << trigTime 
        <<" nHits: " << nHits 
        <<" trigger window: [" << trigTimeLow
        <<", " << trigTimeUp
        <<"] ns " 
        <<endl;
}

// This is based on WCSimWCTriggerBase::AlgNDigits in WCSimWCTrigger.cc
void TriggerAlgo::NDigits(HitTubeCollection *hc, TriggerInfo* ti)
{
    ti->Clear();
    const int nTotalDigiHits = hc->GetTotalNumOfDigiHits();
    int nTriggers = 0;
    float trigTime = 0.;

    if( nTotalDigiHits>0 )
    {
        vector<float> times;
        times.reserve(nTotalDigiHits);
        for(hc->Begin(); !hc->IsEnd(); hc->Next())
        {
            HitTube *aPH = &(*hc)();
            for(int i=0; i<aPH->GetNDigiHits(); i++) 
            {
                times.push_back( aPH->GetTimeDigi(i) );
            }
            aPH = NULL;
        }
        std::sort(times.begin(), times.end());
         
        float tLastHit = times[nTotalDigiHits-1];

        const double stepSize = fNDigitsStepSize; // in ns
        //const double tWindowMax = std::max(0.f, tLastHit - fNDigitsWindow); // in ns

        double tWindowUp = 0.;
        //double tWindowLow = 0.;
        double tWindowLow = times[0];
        if (tWindowLow>0) tWindowLow = ((int)(tWindowLow/stepSize))*stepSize;
        else tWindowLow = ((int)(tWindowLow/stepSize)-1)*stepSize;
        const double tWindowMax = std::max(tWindowLow, (double)(tLastHit - fNDigitsWindow)) + stepSize;
        
        //  - Slide the time window with a width of "fNDigitsWindow"
        //    from "tWindowLow" to "tWindowMax"
        //    with a step size of "stepSize"
        //
        //  - For each step, all the digitized hits falling the corresponding window
        //    are counted. If the number of those hits are greater than "fNDigitsThreshold"
        //    a new trigger is created
        tWindowUp = tWindowLow + fNDigitsWindow;
        int iHit = 0;
        while( tWindowLow<=tWindowMax )
        {
            vector<float> Times;
            Times.clear();
            double next_hit_time = tWindowMax;
            for(iHit=0; iHit<nTotalDigiHits; iHit++)
            {
                float t = times[iHit];
                if( t>=tWindowLow && t<=tWindowUp )
                {
                    Times.push_back( t ); 
                }
                if ( t>tWindowLow && t<next_hit_time ) next_hit_time = t;
            }

            bool isTriggerFound = false;
            if( (int)Times.size()>fNDigitsThreshold )
            {
                trigTime = Times[fNDigitsThreshold];
                if (trigTime>0) trigTime = ((int)(trigTime/stepSize))*stepSize;
                else trigTime = ((int)(trigTime/stepSize)-1)*stepSize;
                float trigTimeLow = trigTime + fPreTriggerWindow[TriggerType::eNDigits];
                float trigTimeUp = trigTime + fPostTriggerWindow[TriggerType::eNDigits];

                // Avoid overlapping with previous trigger window
                if( nTriggers>=1 )
                {
                    float trigTimeUpPrevious = ti->GetUpEdge(nTriggers-1);
                    if( trigTimeUpPrevious>trigTimeLow )
                    { 
                        trigTimeLow = trigTimeUpPrevious;
                    }
                }
                ti->AddTrigger(trigTime,
                               trigTimeLow,
                               trigTimeUp,
                               (int)Times.size(), 
                               (int)TriggerType::eNDigits);
                cout<<" Found trigger at: " << trigTime 
                    <<" nHits: " << Times.size() 
                    <<" trigger window: [" << trigTimeLow
                    <<", " << trigTimeUp
                    <<"] ns " 
                    <<endl;
                isTriggerFound = true;
                nTriggers += 1;
            }
            
            if( isTriggerFound )
            {
                tWindowLow = trigTime + fPostTriggerWindow[TriggerType::eNDigits];
            }
            else
            {
                tWindowLow += stepSize;
                if (tWindowLow<next_hit_time)
                {
                    tWindowLow = next_hit_time;
                    if (tWindowLow>0) tWindowLow = ((int)(tWindowLow/stepSize))*stepSize;
                    else tWindowLow = ((int)(tWindowLow/stepSize)-1)*stepSize;
                }
            }
            tWindowUp = tWindowLow + fNDigitsWindow;
        }
    }

    // Check to see if there is at least one trigger created
    if( nTriggers==0 )
    {
        // No trigger was created 
        // Add a failure tigger
        trigTime = fTriggerTimeForFailure;
        //float trigTimeLow = trigTime - tFirstHit;
        //float trigTimeUp = trigTime + tLastHit;

        float trigTimeLow = trigTime + fPreTriggerWindow[TriggerType::eFailure];
        float trigTimeUp = trigTime + fPostTriggerWindow[TriggerType::eFailure];

        ti->AddTrigger(trigTime,
                       trigTimeLow,
                       trigTimeUp,
                       -1, 
                       (int)TriggerType::eFailure);
        //cout<<" No trigger found " <<endl;
    }
}
