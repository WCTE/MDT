#ifndef WCROOTDATA_IDNUINT_H
#define WCROOTDATA_IDNUINT_H

#include "WCRootDataNuInt.h"
#include "RedNRTVtx_t.h"

class WCRootDataNuInt;

class WCRootDataIDNuInt : public WCRootDataNuInt
{
    public :
        WCRootDataIDNuInt();
        virtual ~WCRootDataIDNuInt();

        void LoadFiles(const char*, const std::vector<string> &v=vector<string>());
        void SetInteractionInformation(PileUpSpill_t*) const;

    private :
        void ReadFriend(const char*);
        void CloseFriend();
        RedNRTVtx_t fNRTVtx;
};
#endif
