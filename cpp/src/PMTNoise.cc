#include "PMTNoise.h"
#include "Configuration.h"
#include "HitTube.h"
#include "HitDigitizer.h"
#include "PMTResponse.h"

PMTNoise::PMTNoise(const int seed, const string &pmtname) :
fNPMTs( 10184 ),
fMinTubeID( 1 ),
fMaxTubeID( 10184 ),
fWinLow( 0. ),
fWinUp( 30000. ),
fWindow( 4000. ),
fDarkRate( 1. ),
fConvRate( 1.126 ),
fDarkMode( 1 ),
fNnoise( 0 ),
fPMTType( pmtname )
{
    fRand = new MTRandom(seed);
    fAftpulse = new PMTAfterpulse();

    fNoisePMT.clear();
    fNoiseTime.clear();

    map<string, string> s;
    s["DarkAddMode"] = "DarkAddMode";
    s["DarkRate"] = "DarkRate";
    s["DarkM0WindowLow"] = "DarkM0WindowLow";
    s["DarkM0WindowUp"] = "DarkM0WindowUp";
    s["DarkM1Window"] = "DarkM1Window";
    s["NumOfTubes"] = "NumOfTubes";
    s["MinTubeID"] = "MinTubeID";
    s["MaxTubeID"] = "MaxTubeID";

    if( fPMTType!="" )
    {
        map<string, string>::iterator i;
        for(i=s.begin(); i!=s.end(); i++)
        {
            i->second += "_" + fPMTType;
        }
    }

    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<int>(s["DarkAddMode"], fDarkMode);
    Conf->GetValue<float>(s["DarkRate"], fDarkRate);
    Conf->GetValue<float>(s["DarkM0WindowLow"], fWinLow);
    Conf->GetValue<float>(s["DarkM0WindowUp"], fWinUp);
    Conf->GetValue<float>(s["DarkM1Window"], fWindow);
	Conf->GetValue<int>(s["NumOfTubes"], fNPMTs);
	Conf->GetValue<int>(s["MinTubeID"], fMinTubeID);
	Conf->GetValue<int>(s["MaxTubeID"], fMaxTubeID);

}

PMTNoise::~PMTNoise()
{
    if( !fRand ){ delete fRand; fRand = NULL;}
    if( fAftpulse ){ delete fAftpulse; fAftpulse = NULL; }
}


void PMTNoise::SetNumberOfPMTs(const int npmts)
{
   fNPMTs = npmts;
}

void PMTNoise::SetMinTubeID(const int tubeID)
{
   fMinTubeID = tubeID;
}

void PMTNoise::SetMaxTubeID(const int tubeID)
{
   fMaxTubeID = tubeID;
}

void PMTNoise::SetWindowLow(const double wlow)
{
    fWinLow = wlow;
}


void PMTNoise::SetWindowUp(const double wup)
{
    fWinUp = wup;
}


void PMTNoise::SetDarkRate(const double rate)
{
    fDarkRate = rate;     
}

void PMTNoise::GenerateDarkNoise(double tWinLow, double tWinUp, bool isDPE, const bool verbose)
{
    fAveNoise=(double)fNPMTs*fDarkRate*(tWinUp - tWinLow);
    // Generate photoelectrons corresponding given dark rate
	// fConvRate: a correction that accounts for the effect of DAQ efficiency
	// NOTE: digi hit having charge < 1 p.e can be rejected by DAQ
    if( isDPE ){ fAveNoise = fAveNoise*fConvRate; } 

    // fDarkRate and windowSize are given in kHz and ns, respectively
    fAveNoise/=1000000.;

    fNoisePMT.clear();
    fNoiseTime.clear();

    fNnoise = fRand->Poisson( fAveNoise );
    fNoisePMT.reserve( fNnoise );
    fNoiseTime.reserve( fNnoise );
    int iTubeID=0;
    for(int i=0; i<fNnoise; i++)
    {
        iTubeID = fMinTubeID + fRand->Integer( fNPMTs );
        if( iTubeID>fMaxTubeID )
        {
            cout<<" [ERROR] PMTNoise::GenerateDarkNoise " <<endl;
            cout<<"   - Generated tube ID:" << iTubeID <<endl;
            cout<<"   - EXceeding max tube ID:" << fMaxTubeID <<endl;
            cout<<" -> EXIT " <<endl;
            exit(-1);
        }
        fNoisePMT[i] = iTubeID;
        fNoiseTime[i] = fRand->Uniform(tWinLow, tWinUp);
    } 

    if( verbose )
    {
        cout<<" [INFO] PMTNoise::GenerateDarkNoise " <<endl;
        cout<<"   - Window Low:" << fWinLow <<" (ns)" <<endl;
        cout<<"   - Window Up:" << fWinUp <<" (ns)" <<endl;
        cout<<"   - # PMTs:" << fNPMTs <<endl;
        cout<<"   - Dark rate:" << fDarkRate <<" (kHz)" <<endl;
        cout<<"   - Mean # noise hits:" << fAveNoise <<endl;
        cout<<"   - # generated noise hits:" << fNnoise <<endl; 
    }
}

int PMTNoise::GetNumberOfGeneratedDarkHits() const
{
    return fNnoise;
}

void PMTNoise::AddPhotoElectrons(HitTubeCollection* hc)
{
    // Get correct PMT numbers each time
    fNPMTs = hc->GetHitTubeIDList().size();
    fMinTubeID = 0;
    fMaxTubeID = fNPMTs-1;

    vector< pair<double, double> > range;
    if( fDarkMode==1 )
    {
        this->FindRanges(hc, range);
    }
    else 
    {
        range.push_back( pair<double, double>(fWinLow, fWinUp) );
    }

    for(unsigned int i=0; i<range.size(); i++) 
    {
        this->Add(hc, range[i].first, range[i].second);
    }
}

// Based on WCSimWCAddDarkNoise::FindDarkNoiseRanges in WCSimWCAddDarkNoise.cc
void PMTNoise::FindRanges(HitTubeCollection *hc, vector<pair<double, double>> &range)
{
// - Add photoelectrons corresponding dark current 
//   to each true Cherenkov hit around +- "fWindow"/2 
// - Here these time windows are calculated

    range.clear();
    const int nTrueHits = hc->GetTotalNumOfTrueHits();
    vector<pair<double, double>> tmp;
    tmp.reserve(nTrueHits);

    for(hc->Begin(); !hc->IsEnd(); hc->Next())
    {
        HitTube *aPH = &(*hc)();
        const int nPE = aPH->GetNRawPE();
        for(int i=0; i<nPE; i++)
        {
            float t = aPH->GetTimeRaw(i);
            double tLow = t - fWindow/2.;
            double tUp = t + fWindow/2.;
            tmp.push_back(pair<double, double>(tLow, tUp));
        }
    }

    // If no true hits are stored, add a dummy such that no dark noise is added in later stage
    if( tmp.size()==0 )
    {
        range.push_back(pair<double,double>(0., 0.));
    }
    else
    {
        // Sort the "tmp" vector in increasing order, then check whether or not 
        // the windows defined above overlapp with other window 
        std::sort(tmp.begin(), tmp.end());

        vector<pair<double, double>>::iterator itr2 = tmp.begin();
        pair<double, double> cur = *(itr2)++;
        for(; itr2!=tmp.end(); itr2++) 
        {
            if( cur.second>=itr2->first )
            {
                cur.second = std::max(cur.second,itr2->second);
            } 
            else
            {
                range.push_back( cur );
                cur = *(itr2);
            }
        }
        range.push_back( cur );
    } 
}

void PMTNoise::Add(HitTubeCollection *hc, double tWinLow, double tWinUp)
{
    this->GenerateDarkNoise(tWinLow, tWinUp, true, false);
    // Get correct tubeID matching
    const vector<int>& hitTubeIDList = hc->GetHitTubeIDList();
    const int nDarkHits = this->GetNumberOfGeneratedDarkHits();
    for(int k=0; k<nDarkHits; k++)
    {
        int tubeID = hitTubeIDList.at(this->GetNoiseTube(k));
        float time = this->GetNoiseTime(k); 
		hc->AddTrueHit(tubeID, time, -1);
    }
    //cout<<" NumCkovPE(dark PE): " << nDarkHits <<endl;
}


void PMTNoise::AddAfterpulse(HitTubeCollection *hc, HitDigitizer *hd, PMTResponse *pr)
{
    int NDigiHitsBf = hc->GetTotalNumOfDigiHits();
    int NAP = 0;
    int NDigi = 0;
    for(hc->Begin(); !hc->IsEnd(); hc->Next())
    {
        HitTube *aPH = &(*hc)();
        NDigi += aPH->GetNDigiHits();

        // Store digitize hits due to afterpluse temporally
        HitTube *aPHAP = new HitTube(aPH->GetTubeID());
        for(int i=0; i<aPH->GetNDigiHits(); i++)
        {
            float charge = aPH->GetChargeDigi(i);
            const vector<int> composition = aPH->GetParentCompositionDigi(i);
            if( fAftpulse->GenerateAfterpulse(charge, composition, fRand) )
            {
                double charge_ap = pr->GetRawSPE();
                bool pass = false;
                hd->ApplyThreshold(charge_ap, pass);
                while( !pass )
                {
                    charge_ap = pr->GetRawSPE();
                    hd->ApplyThreshold(charge_ap, pass);
                }

                float time_ap = fAftpulse->GetAfterpulseTime(aPH->GetTimeDigi(i), fRand);
                vector<int> comp_ap(1, -2);
                aPHAP->AddDigiHit(time_ap, charge_ap, comp_ap);
                NAP += 1;
            }
        }

        for(int i=0; i<aPHAP->GetNDigiHits(); i++)
        {
            float time = aPHAP->GetTimeDigi(i); 
            float charge = aPHAP->GetChargeDigi(i);
            const vector<int> comp = aPHAP->GetParentCompositionDigi(i);
            aPH->AddDigiHit(time, charge, comp);
        }
    }

    int NDigiHitsAf = hc->GetTotalNumOfDigiHits();
    cout<<" NDigiHits(w/o AP): " << NDigiHitsBf 
        <<" NDigiHits(w/ AP): " << NDigiHitsAf 
        <<" NAP: " << NAP 
        <<" NDigi: " << NDigi
        <<endl;
}
