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

void FBPlotter::RGBEncode(string filename, vector<vector<int> > p, vector<vector<int> > f) {
    // TODO
}

void FBPlotter::Plot(string filename) {
    try {
        const unsigned long len = iW*iH;
        int mag = 0;
        int fbmag = 0;
        // encode matrix data into file
        std::remove("rgbData");
        std::ofstream rgbData;
        rgbData.open("rgbData");
        for(unsigned long iIndex = 0; iIndex < len; iIndex++)
        {
            int x = iIndex%iW;
            int y = iIndex/iW;
            mag = analysis[x][y]/129;
            fbmag = FBProbs[x][y]/129;
            rgbData << x << " " << y << " " << fbmag << " " << mag << " " << mag << std::endl;

            QCoreApplication::processEvents();
        }
        rgbData.close();
        Gnuplot g9;
        g9.set_xrange(0,iW-1).set_yrange(0,iH).set_cbrange(0,255);
        //g9.cmd("set palette color");
        g9.plot_rgbimage(filename,iW,iH,"Spectrum");
    }
    catch (GnuplotException ge) {
        std::cout << ge.what() << endl;
    }
}
