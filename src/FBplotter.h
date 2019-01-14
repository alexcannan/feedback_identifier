#ifndef FBPLOTTER
#define FBPLOTTER

#include <string>
#include <vector>

using namespace std;

class FBPlotter
{
    public:
        FBPlotter(vector<vector<int> > p, vector<vector<int> > f);
        ~FBPlotter();
        void RGBEncode(const char* filename, vector<vector<int> > p, vector<vector<int> > f);
        void Plot(string filename, bool isKyoto);

    private:
        unsigned int iWprob;
        unsigned int iHprob;
        unsigned int iWfft;
        unsigned int iHfft;

};
#endif // FBPLOTTER

