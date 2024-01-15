#include "WCRootDataNuInt.h"

//WCRootDataNuInt::WCRootDataNuInt(int seed)
WCRootDataNuInt::WCRootDataNuInt()
{
    fRnd = new TRandom3(7698);
//    fRnd = new TRandom3(seed);
    fFriendC = 0;
    fFileList.clear();
    fFriendList.clear();

    fHasFriend = false;
    fNuIntType = eSingle;

    fItrEntry = 0;
    fItrFile = 0;
    fNumEntries = 0;
    fNumFiles = 0;
    fFileIdNum = -9999;
    fIntRate = 0.;
    fNDigitsFileIdNum = 5;
}

WCRootDataNuInt::~WCRootDataNuInt()
{
    delete fRnd; fRnd = 0;
    if( fFriendC ){ delete fFriendC; fFriendC = 0; }
    fFileList.clear();
    fFriendList.clear();
    std::vector<pair<TString, int>>().swap(fFileList);
    std::vector<pair<TString, int>>().swap(fFriendList);
}

void WCRootDataNuInt::LoadFiles(const char *filename, const vector<string> &list)
{
    ifstream fin(filename);
    string aLine;
    string sname;
    string svalue;
    int  num = -1;
    while( std::getline(fin, aLine) )
    {
        stringstream ss(aLine);
        string stmp;
        ss>>stmp;
        //num = this->GetFileIdNumber(stmp.c_str());
        num++;
        fFileList.push_back(pair<TString, int>(stmp, num));
        std::cout<<" Adding " << stmp <<" " << num <<std::endl;
        //while( ss>>stmp )
        //{
        //    std::cout<<" stmp " << stmp <<std::endl;
        //    if( stmp.find('wcsim')!=string::npos )
        //    {
        //        fFileList.push_back(stmp);
        //    }
        //}
    }
    fin.close();

    fNumFiles = fFileList.size();
    std::cout<<" Found " << fNumFiles <<" files " <<std::endl;

    fWCRootEvtList = list;
    
    fItrFile = fRnd->Integer(fNumFiles);
    this->ReadFile(fFileList[fItrFile].first,fWCRootEvtList);
    fFileIdNum = fFileList[fItrFile].second;
    fNumEntries = this->GetEntries();
    fItrEntry = fRnd->Integer(fNumEntries);

    std::cout<<" Starting from: " << fFileList[fItrFile].first
             <<" " << fFileList[fItrFile].second
             <<" " << fItrEntry
             <<"/" << fNumEntries
             <<std::endl;
}


bool WCRootDataNuInt::Next()
{
    if( fItrEntry>=fNumEntries )
    {
        this->CloseFile();
        if( fHasFriend ){ this->CloseFriend(); };
        fItrFile += 1;
        if( fItrFile==fNumFiles  )
        {
            fItrFile = fRnd->Integer(fNumFiles);
            std::cout<<" Rewiding file from " << fFileList[fItrFile].first <<std::endl;
        }
        this->ReadFile(fFileList[fItrFile].first,fWCRootEvtList);
        if( fHasFriend ){ this->ReadFriend(fFriendList[fItrFile].first); };

        fFileIdNum = fFileList[fItrFile].second;
        fNumEntries = this->GetEntries();
        fItrEntry = fRnd->Integer(fNumEntries);

        std::cout<<" Reading " << fFileList[fItrFile].first 
                 <<" " << fFileList[fItrFile].second
                 <<" " << fItrEntry 
                 <<"/" << fNumEntries 
                 <<std::endl;
    }
    this->GetEntry(fItrEntry);
    if( fHasFriend ){ fFriendC->GetEntry(fItrEntry); }
    fItrEntry++;

    return true;
}

int WCRootDataNuInt::GetFileIdNumber(const char *filename, const char *prefix)
{
	const int size_of_num = fNDigitsFileIdNum;
	TString InFileName(filename);
	int total_size = InFileName.Sizeof() - 1;
	TString sPrefix(prefix);
	TString sROOT(".root");
	int index_Prefix = InFileName.Index( sPrefix );
	int len_Prefix = sPrefix.Sizeof() - 1;
	int len_ROOT = sROOT.Sizeof() - 1;
	int reco_total_size = index_Prefix + len_Prefix + size_of_num + len_ROOT;

	// Check if the file name is acceptable
	if( reco_total_size!=total_size )
	{
		std::cout<<" [ERROR] WCRootDataNuInt::GetFileIdNumber " <<std::endl;
		std::cout<<"  - Input file name is not formatted in: -----" << sPrefix <<"XXXXX.root " <<std::endl;
		std::cout<<"  , where XXXX assumed to be 00000 - 99999 " <<std::endl;
		std::cout<<" Cannot obtain file number for: " << InFileName <<std::endl;
		std::cout<<" -> EXIT "<< std::endl;
		exit(-1);
	}
	int index_start = index_Prefix + sPrefix.Sizeof();
	TString sNum = InFileName(index_start, size_of_num);
	return atoi( sNum );
}
