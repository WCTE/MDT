// Store the following information
// - Number of event triggers 
// - Triggered time
// - Lower edge
// - Upper edge
// - Trigger type

#pragma once

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

enum class TriggerType : int { eUndefined = -1,
                               eNDigits,
                               eNDigitsTest,
                               eTestVertices,
                               eNoTrig,
                               eFailure};

class TriggerInfo
{
    public:  
        TriggerInfo();
        virtual ~TriggerInfo();

        void AddTrigger(float, float, float, int, int);
        void Clear(); 

        int GetNumOfTrigger() const { return (int)fTrigTime.size(); }
        float GetTriggerTime(int i) const { return fTrigTime[i]; }
        float GetLowEdge(int i) const { return fTimeLow[i]; }
        float GetUpEdge(int i) const { return fTimeUp[i]; }
        int GetNHits(int i) const { return fNHits[i]; }
        int GetType(int i) const { return fTrigType[i]; }
        
    private:
        vector<float> fTrigTime;
        vector<float> fTimeLow;
        vector<float> fTimeUp;
        vector<int> fNHits;
        vector<int> fTrigType; 
};
