#pragma once

#include "PMTResponse.h"

class PMTResponse3inchR12199_02 : public GenericPMTResponse
{
    public: 
        PMTResponse3inchR12199_02(int, const string &s="");
        PMTResponse3inchR12199_02();
        virtual ~PMTResponse3inchR12199_02();
        float HitTimeSmearing(float);
        void Initialize(int, const string &s="");
};
