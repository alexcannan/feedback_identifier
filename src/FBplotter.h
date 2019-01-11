#ifndef FBPLOTTER
#define FBPLOTTER

#include <string>
#include <vector>

using namespace std;

class FBPlotter
{
    public:
        FBPlotter(vector<vector<int> > m);
        ~FBPlotter();
        void Matrix2File(string filename, vector<vector<int> > matrix);
        void Plot();

    private:
        unsigned int iW;
        unsigned int iH;

};
#endif // FBPLOTTER

