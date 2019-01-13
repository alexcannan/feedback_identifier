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
        void Matrix2File(string filename, vector<vector<int> > p, vector<vector<int> > f);
        void Plot();

    private:
        unsigned int iWprob;
        unsigned int iHprob;
        unsigned int iWfft;
        unsigned int iHfft;

};
#endif // FBPLOTTER

