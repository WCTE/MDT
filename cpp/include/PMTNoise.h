#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <map>
#include <string>

#include "MTRandom.h"
#include "HitTubeCollection.h"
#include "PMTAfterpulse.h"

using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::map;
using std::string;

class HitDigitizer;
class PMTResponse;

class PMTNoise 
{
    public :
        PMTNoise(const int, const string &s="");
        virtual ~PMTNoise();

        void SetNumberOfPMTs(const int);
        void SetMinTubeID(const int);
        void SetMaxTubeID(const int);
        void SetWindowLow(const double /* ns */);
        void SetWindowUp(const double /* ns */);
        void SetDarkRate(const double /* kHz */);
        void SetDarkMode(const int i){ fDarkMode = i; }

        void GenerateDarkNoise(double, double, bool isDPE=false, const bool verbose=false);
        int GetNumberOfGeneratedDarkHits() const;
        int GetNumberOfGeneratedDarkPhotoElectrons() const;
        int GetNoiseTube(const int i) const { return fNoisePMT[i]; }
        double GetNoiseTime(const int i) const { return fNoiseTime[i]; }

        void AddPhotoElectrons(HitTubeCollection*);
        void Add(HitTubeCollection*, double, double);
        void FindRanges(HitTubeCollection*, vector<pair<double, double>>&);

        void AddAfterpulse(HitTubeCollection*, HitDigitizer*, PMTResponse *pr);

    private :
        MTRandom *fRand;
        int fNPMTs;
        int fMinTubeID;
        int fMaxTubeID;
        float fWinLow;
        float fWinUp;
        float fWindow;
        float fDarkRate;
        float fAveNoise;
        float fConvRate;
        int fDarkMode;

        int fNnoise;
        vector<int> fNoisePMT;
        vector<double> fNoiseTime;

        PMTAfterpulse *fAftpulse;

        string fPMTType;
};
