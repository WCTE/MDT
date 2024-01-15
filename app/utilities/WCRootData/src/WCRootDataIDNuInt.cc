#include "WCRootDataIDNuInt.h"

WCRootDataIDNuInt::WCRootDataIDNuInt()
{
    fHasFriend = false;
    fNuIntType = eIDNuInt;
};

WCRootDataIDNuInt::~WCRootDataIDNuInt()
{
}

void WCRootDataIDNuInt::LoadFiles(const char *filename, const vector<string> &list)
{
    // Read input file list of WCSim first
    this->WCRootDataNuInt::LoadFiles(filename,list);

    if (!fHasFriend) return;

    ifstream fin(filename);
    string aLine;
    string sname;
    string svalue;
    int num = -1;
    while( std::getline(fin, aLine) )
    {
        stringstream ss(aLine);
        string stmp;
        ss>>stmp;
        ss>>stmp;
        num = this->GetFileIdNumber(stmp.c_str(), "rednrtvtx.");
        fFriendList.push_back(pair<TString, int>(stmp, num));
    }
    fin.close();
    this->ReadFriend(fFriendList[fItrFile].first);
}

void WCRootDataIDNuInt::ReadFriend(const char *filename)
{
    cout<<" RedNRTVtxT reading: " << filename <<endl;
    fFriendC = new TChain("RedNRTVtxT");
    fFriendC->Add(filename);
    fNRTVtx.SetBranch(fFriendC);
}

void WCRootDataIDNuInt::CloseFriend()
{
    delete fFriendC; fFriendC = 0;
}


void WCRootDataIDNuInt::SetInteractionInformation(PileUpSpill_t *p) const
{
    p->Mode[p->NNuIntTot] = fNRTVtx.Mode[0];
    p->NuPDG[p->NNuIntTot] = fNRTVtx.NuPDG[0];
    p->NuEne[p->NNuIntTot] = fNRTVtx.NuEne[0];
    for(int i=0; i<3; i++)
    {
        p->NuPos[p->NNuIntTot][i] = fNRTVtx.NuPos[0][i];
        p->NuDir[p->NNuIntTot][i] = fNRTVtx.NuDir[0][i];
    }
}
