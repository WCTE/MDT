#ifndef WCROOTDATA_BEAMBKG_H
#define WCROOTDATA_BEAMBKG_H

#include "WCRootDataNuInt.h"

class WCRootDataNuInt;

class WCRootDataBeamBkg : public WCRootDataNuInt
{
    public :
        WCRootDataBeamBkg();
        virtual ~WCRootDataBeamBkg();

        void LoadFiles(const char *, const std::vector<string> &v=vector<string>());
        float GetEnergyDepositionInOD() const { return fEdepoOD; }

    private :
        void ReadFriend(const char*);
        void CloseFriend();

        Int_t fgrNPart;
        Float_t fEdepoOD;
};
#endif
