#ifndef FFTANALYZER_H
#define FFTANALYZER_H
#define PI 3.14159265358979
#define FIXED_POINT 16
#include <vector>

using namespace std;

class FFTAnalyzer
{
    public:
        FFTAnalyzer(unsigned vS, unsigned aP, unsigned sR);
        ~FFTAnalyzer();
        vector<vector<int> > fileAnalyze(vector<int> data);

        vector<vector<int> > signaldata;

    private:
        unsigned inputLength;
        unsigned vecSize;
        unsigned buffCount;
        unsigned buffLeftover;
        unsigned analysisPeriod;
        unsigned sampleRate;

};

#endif // FFTANALYZER_H
