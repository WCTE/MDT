#pragma once

class TrueHit
{
    public:
        TrueHit(float t, int id=999999999);
        TrueHit();
        virtual ~TrueHit();

        float GetTime() const { return fTime; }
        float GetParentId() const { return fParentId; }
        float GetPosition(int i) const { return fPosition[i]; }
        float GetDirection(int i) const { return fDirection[i]; }
        float GetStartTime() const {return fStartTime; }
        float GetStartPosition(int i) const { return fStartPosition[i]; }
        float GetStartDirection(int i) const { return fStartDirection[i]; }
        int GetPosBin(int i) const { return fBin[i]; }

        void SetPosition(int i, float f) { fPosition[i] = f; }
        void SetDirection(int i, float f) { fDirection[i] = f; }
        void SetStartTime(float f) { fStartTime = f; }
        void SetStartPosition(int i, float f) { fStartPosition[i] = f; }
        void SetStartDirection(int i, float f) { fStartDirection[i] = f; }
        void SetPosBin(int i, int b){ fBin[i] = b; }

    private:
        float fTime;
        float fPosition[3]; // Hit position on photocade
        float fDirection[3]; // Direction of photon hitting photocathode
        float fStartTime; // Photon track initial time
        float fStartPosition[3]; // Photon track initial position
        float fStartDirection[3]; // Photon track initial direction
        int fParentId;
        int fBin[3];
};
