#ifndef WCROOTDATANUINT_H
#define WCROOTDATANUINT_H

#include <sstream>
#include "TRandom3.h"
#include "WCRootData.h"
#include "PileUpSpill_t.h"

using std::stringstream;

class WCRootData;

enum ENuIntType{ eIDNuInt=0,
                 eBeamBkg,
                 eSingle
               };

class WCRootDataNuInt : public WCRootData
{
    public:
        WCRootDataNuInt();
        virtual ~WCRootDataNuInt();

        void SetSeed(const int i){ fRnd->SetSeed(i); }
        void LoadFiles(const char*, const std::vector<string> &v=vector<string>()); 
        virtual bool Next();

        void SetNDigitsFileIDNumber(int i){ fNDigitsFileIdNum = i; }
        int GetEventIdNumber() const { return fItrEntry-1; }
        int GetFileIdNumber() const { return fFileIdNum; }

        void SetInteractionRate(const float f)  { fIntRate = f; }
        int DrawNumOfInteraction() const { return fRnd->Poisson(fIntRate); }

        unsigned GetInteractionType() const { return fNuIntType; }
        virtual float GetEnergyDepositionInOD() const { return 0.; }
        virtual void SetInteractionInformation(PileUpSpill_t*) const {};

        void SetHasFriend(bool val) {fHasFriend=val;}

    protected:
        int GetFileIdNumber(const char*, const char *prefix="wcsim.");

        virtual void ReadFriend(const char* f){ cout<<" Reading friend: " << f <<endl; };
        virtual void CloseFriend(){};

        TRandom3 *fRnd;
        std::vector<pair<TString, int>> fFileList; 
        std::vector<pair<TString, int>> fFriendList; 
        TChain *fFriendC;

        bool fHasFriend;
        unsigned int fNuIntType;
        

        int fItrEntry;          
        int fItrFile;
        int fNumEntries;
        int fNumFiles;
        int fFileIdNum; 
        int fNDigitsFileIdNum;
        float fIntRate;
        std::vector<string> fWCRootEvtList;
};
#endif
