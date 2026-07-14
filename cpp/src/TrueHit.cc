#include "TrueHit.h"

TrueHit::TrueHit(double t, int id)
{
    fTime = t;
    fParentId = id;
    fPosition[0] = 0.;
    fPosition[1] = 0.;
    fPosition[2] = 0.; 
};

TrueHit::TrueHit()
{
    fPosition[0] = 0.; 
    fPosition[1] = 0.;
    fPosition[2] = 0.; 
};

TrueHit::~TrueHit()
{
};
