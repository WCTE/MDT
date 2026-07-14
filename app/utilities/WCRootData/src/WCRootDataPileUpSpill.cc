#include "WCRootDataPileUpSpill.h"
#include "WCRootDataNuInt.h"

WCRootDataPileUpSpill::WCRootDataPileUpSpill()
{
    fOutFileNum = 0; 
    fOutFilePrefix = "";
    fNumOfSpillsPerFile = 1000; 
    fCurSpill = 0;
    fPupSpillC = 0;

    fPupSpillTreeName = "pupspillT";

    fFileNameToBeCopied = "";
}

WCRootDataPileUpSpill::~WCRootDataPileUpSpill()
{
    if( fPupSpillC ){ delete fPupSpillC; fPupSpillC = 0; }
}

void WCRootDataPileUpSpill::ReadFile(const char *filename)
{
    fPupSpillC = new TChain(fPupSpillTreeName);
    fPupSpillC->Add(filename);
    fPupSpill.Clear();
    fPupSpill.SetBranch(fPupSpillC);
    this->WCRootData::ReadFile(filename);
}


void WCRootDataPileUpSpill::CloseFile()
{
    delete fPupSpillC; fPupSpillC = 0;
    this->WCRootData::CloseFile();
}

int WCRootDataPileUpSpill::GetEntries()
{
    if( this->WCRootData::GetEntries()!=fPupSpillC->GetEntries() ) 
    {
        cout<<" Number of enetries are different" <<endl;
        cout<<"  - wcsimT: " << this->WCRootData::GetEntries() <<endl;
        cout<<"  - " << fPupSpillTreeName <<": " << fPupSpillC->GetEntries() <<endl;
    }
    return this->WCRootData::GetEntries();
}

void WCRootDataPileUpSpill::GetEntry(const int i)
{
    fPupSpillC->GetEntry(i);
    this->WCRootData::GetEntry(i);
}

void WCRootDataPileUpSpill::GetOutFileIdNumber(const int i, TString &s)
{
    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') <<i;
    s = TString(ss.str());
}

void WCRootDataPileUpSpill::CreateTree(const char *prefix, const vector<string> &list)
{
    TString sOutFileNum = "";
    this->GetOutFileIdNumber(fOutFileNum, sOutFileNum);

    if( prefix!=0 ){ fOutFilePrefix = TString(prefix); }
    TString OutFileName = fOutFilePrefix + "." + sOutFileNum + ".root";
    std::cout<<" Creating new output file: " << OutFileName <<std::endl;
    fOutFileNum += 1;
    this->WCRootData::CreateTree(OutFileName,list);

    TFile *f = fWCSimT->GetCurrentFile();
    fPupSpillT = new TTree(fPupSpillTreeName, "Pile-up spill tree");
    fPupSpillT->SetDirectory(f);
    fPupSpill.CreateBranch(fPupSpillT);
    fPupSpill.Clear();

    fCurSpill = 0;
}

void WCRootDataPileUpSpill::FillTree()
{
    if( fCurSpill==fNumOfSpillsPerFile )
    {
        this->WriteTree();
        this->CreateTree();
    }

    TFile *f = fWCSimT->GetCurrentFile();
    f->cd();
    fWCSimT->Fill();     
    fWCSimT->Write("", TObject::kOverwrite);
    fPupSpillT->Fill();
    fPupSpillT->Write("", TObject::kOverwrite);

    for(unsigned int i=0; i<fSpEvt.size(); i++)
    {
        fSpEvt[i]->ReInitialize();
    }
    fPupSpill.Clear();
    
    fCurSpill += 1;
}

void WCRootDataPileUpSpill::WriteTree()
{
    TFile *f = fWCSimT->GetCurrentFile();
    this->WCRootData::SetOutFileName(f->GetName());
    f->cd();

    fWCSimT->Write();
    fPupSpillT->Write();

    f->Close();

    if( fFileNameToBeCopied!="" )
    {
        this->CopyTree(fFileNameToBeCopied, "wcsimGeoT");
        this->CopyTree(fFileNameToBeCopied, "Settings");
        this->CopyTree(fFileNameToBeCopied, "wcsimRootOptionsT");
    }
}

void WCRootDataPileUpSpill::AddInteraction(const WCRootDataNuInt *aNuInt,
                                           float offset_time,
                                           int bunch_id)
{
    fPupSpill.NuIntType[fPupSpill.NNuIntTot] = aNuInt->GetInteractionType();
    fPupSpill.NuIntTime[fPupSpill.NNuIntTot] = offset_time;
    fPupSpill.NuIntBunch[fPupSpill.NNuIntTot] = bunch_id; 
    fPupSpill.EdepoOD[fPupSpill.NNuIntTot] = aNuInt->GetEnergyDepositionInOD();
    fPupSpill.FileIdNum[fPupSpill.NNuIntTot] = aNuInt->GetFileIdNumber();
    fPupSpill.EventIdNum[fPupSpill.NNuIntTot] = aNuInt->GetEventIdNumber();
    aNuInt->SetInteractionInformation(&fPupSpill);

    this->AddTracks(aNuInt->GetTrigger(0,0), offset_time, 0);

    const int iFirst = fPupSpill.NTracks;
    const int nTrackTmp = ((TClonesArray*)fSpEvt[0]->GetTrigger(0)->GetTracks())->GetEntries();
    for(int i=iFirst; i<nTrackTmp; i++)
    {
        fPupSpill.TrackNuIntIdx[i] = fPupSpill.NNuIntTot;
        fPupSpill.NTracks += 1;    
    }

    fPupSpill.NNuIntTot += 1;
    if( aNuInt->GetInteractionType()==0 ){ fPupSpill.NIDNuInt += 1; }
    else if( aNuInt->GetInteractionType()==1 ){ fPupSpill.NBeamBkg += 1; }
}

void WCRootDataPileUpSpill::AddDigiHitsToMDT(MDTManager *mdt,
                                             float time_low,
                                             float time_up,
                                             float mean_bunch_time)
{
    const WCSimRootTrigger* aEvt = this->WCRootData::GetTrigger(0);
    const TClonesArray *allDigiHits = aEvt->GetCherenkovDigiHits();
    const int nAllDigiHits = allDigiHits->GetEntries();

    HitTubeCollection *hc = mdt->GetHitTubeCollection();
    for(int i=0; i<nAllDigiHits; i++)
    {
        WCSimRootCherenkovDigiHit *aHit
            = (WCSimRootCherenkovDigiHit*)allDigiHits->At(i);

        float t = aHit->GetT();
       
        int tube_id = aHit->GetTubeId();
        float q = aHit->GetQ();
        // int mPMT_module_id = aHit->GetmPMTId();
        // int mPMT_pmt_id = aHit->GetmPMT_PMTId();
        vector<int> true_hit_ids = aHit->GetPhotonIds();

        if( !hc->HasTube(tube_id) )
        {
            hc->AddHitTube(tube_id);
        }
        (&(*hc)[tube_id])->AddDigiHit(t, q, true_hit_ids);
    }
    cout<<" Total - before adding AP: " << hc->GetTotalNumOfDigiHits() <<endl;
    hc = 0;
    hc = mdt->GetHitTubeCollection();
    mdt->DoAddAfterpulse();
    cout<<" Total - after adding AP: " << hc->GetTotalNumOfDigiHits() <<endl;


    HitTubeCollection *hcSel = new HitTubeCollection();
    int n = 0;
    for(hc->Begin(); !hc->IsEnd(); hc->Next())
    {
        HitTube *aPH = &(*hc)();
        int tube_id = aPH->GetTubeID();
        for(int i=0; i<aPH->GetNDigiHits(); i++)
        {
            float t = aPH->GetTimeDigi(i);
            if( !(t>time_low && t<time_up) ){ continue; }

            vector<int> true_hit_ids = aPH->GetParentCompositionDigi(i);
            t = t - mean_bunch_time;
            float q = aPH->GetChargeDigi(i);

            if( !hcSel->HasTube(tube_id) )
            {
                hcSel->AddHitTube(tube_id);
            }
            (&(*hcSel)[tube_id])->AddDigiHit(t, q, true_hit_ids);
        } // digi hit loop
        aPH = NULL;
        n++;
    } // PMT loop
    cout<<" Selected with adding AP: " << hcSel->GetTotalNumOfDigiHits() <<endl;
    mdt->SetHitTubeCollection(hcSel);
    hc = 0;
    hcSel = 0;
}
