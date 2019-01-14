#include "FBplotter.h"

#include "lib/gnuplot_i.hpp"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

FBPlotter::FBPlotter(vector<vector<int> > p, vector<vector<int> > f) {
    iWprob = p.size();
    iHprob = p[0].size();
    iWfft = f.size();
    iHfft = f[0].size();
}

FBPlotter::~FBPlotter() {
    // dtor
}

void FBPlotter::RGBEncode(const char* filename, vector<vector<int> > p, vector<vector<int> > f) {

    const unsigned long len = iWfft*iHfft;
    int fft = 0;
    int prob = 0;
    // encode matrix data into file
    std::remove(filename);
    std::ofstream rgbData;
    rgbData.open(filename);
    for(unsigned long iIndex = 0; iIndex < len; iIndex++)
    {
        int x = iIndex%iWfft;
        int y = iIndex/iWfft;
        fft = f[x][y]/129;
        if (x < iWprob) {
            prob = p[x][y]/129;
        } else {
            prob = 0;
        }
        rgbData << x << " " << y << " " << prob << " " << fft << " " << fft << std::endl;
    }
    rgbData.close();

}

void FBPlotter::Plot(string filename, bool isKyoto) {
    try {
        Gnuplot g9;
        g9.set_xrange(0,iWfft-1).set_cbrange(0,255);
        if (isKyoto) {g9.set_yrange(0,iHfft/4);} else {g9.set_yrange(0,iHfft/2);}
        //g9.cmd("set palette color");
        g9.plot_rgbimage(filename,iWfft,iHfft,"Spectrum");
    }
    catch (GnuplotException ge) {
        std::cout << ge.what() << endl;
    }
}
