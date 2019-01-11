#include "FBplotter.h"

#include "lib/gnuplot_i.hpp"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

FBPlotter::FBPlotter(vector<vector<int> > m) {
    iW = m.size();
    iH = m[0].size();
}

FBPlotter::~FBPlotter() {
    // dtor
}

void FBPlotter::Matrix2File(string filename, vector<vector<int> > matrix) {
    // TODO
}

void FBPlotter::Plot() {
    // TODO
}
