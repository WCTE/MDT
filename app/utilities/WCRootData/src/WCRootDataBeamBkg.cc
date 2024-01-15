#include "WCRootDataBeamBkg.h"

WCRootDataBeamBkg::WCRootDataBeamBkg()
{
    fHasFriend = false;
    fNuIntType = eBeamBkg;
};

WCRootDataBeamBkg::~WCRootDataBeamBkg()
{
}

void WCRootDataBeamBkg::LoadFiles(const char *filename, const vector<string> &list)
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
        num = this->GetFileIdNumber(stmp.c_str(), "redgene.");
        fFriendList.push_back(pair<TString, int>(stmp, num));
    }
    fin.close();
    this->ReadFriend(fFriendList[fItrFile].first);
}

void WCRootDataBeamBkg::ReadFriend(const char *filename)
{
    fFriendC = new TChain("geneT");
    //fFriendC->SetBranchStatus("*", 0);
    //fFriendC->SetBranchStatus("EdepoOD", 1);
    //fFriendC->SetBranchStatus("grNPart", 1);
    fFriendC->SetBranchAddress("EdepoOD", &fEdepoOD);
    fFriendC->SetBranchAddress("grNPart", &fgrNPart);
    fFriendC->Add(filename);
}

void WCRootDataBeamBkg::CloseFriend()
{
    delete fFriendC; fFriendC = 0;
}
