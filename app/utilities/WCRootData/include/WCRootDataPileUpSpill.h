#ifndef WCROOTDATAPILEUPSPILL_H
#define WCROOTDATAPILEUPSPILL_H

#include <sstream>
#include <iomanip>

#include "WCRootData.h"
#include "PileUpSpill_t.h"

using std::stringstream;
class WCRootDataNuInt;

class WCRootDataPileUpSpill : public WCRootData
{
    public:
        WCRootDataPileUpSpill();
        virtual ~WCRootDataPileUpSpill();

        // For reading
        void ReadFile(const char*);
        void CloseFile();
        int GetEntries();
        void GetEntry(const int);
        const PileUpSpill_t *GetPileUpSpill() const {return &fPupSpill;}

        void AddDigiHitsToMDT(MDTManager*, float, float, float);

        // For writing 
       // void SetOutFilePrefix(const char *f){ fOutFilePrefix = TString(f); }
        void CreateTree(const char *f=0, const vector<string> &v=vector<string>());
        void FillTree();
        void WriteTree();
        void AddInteraction(const WCRootDataNuInt*, float offset_time=0., int bunch_id=99999);

        void SetNumOfSpillesSavedPerFile(const int i){ fNumOfSpillsPerFile = i; }
        void SetFileNameForCopyTree(const char *f){ fFileNameToBeCopied = TString(f); }

    private:
        void GetOutFileIdNumber(const int, TString&);

        TString fOutFilePrefix;
        TString fPupSpillTreeName;
        int fOutFileNum; 
        int fNumOfSpillsPerFile;
        int fCurSpill;

        PileUpSpill_t fPupSpill;
        TTree *fPupSpillT;
        TChain *fPupSpillC;

        TString fFileNameToBeCopied;
};
#endif
