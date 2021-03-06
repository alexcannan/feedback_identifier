#include "FFTAnalyzer.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <kiss_fft130/kiss_fft.h>
#include <kiss_fft130/_kiss_fft_guts.h>
#include <kiss_fft130/tools/kiss_fftr.h>

extern "C" {
#include "lib/fft4g.h"
}

using namespace std;

FFTAnalyzer::FFTAnalyzer(unsigned vS, unsigned aP, unsigned sR): vecSize(vS), analysisPeriod(aP), sampleRate(sR) {
    cout << "Beginning " << vecSize << "-pt analysis of " << sampleRate << " Hz signal" << endl;
}

FFTAnalyzer::~FFTAnalyzer() {
    //dtor
}

std::vector<std::vector<int> > FFTAnalyzer::fileAnalyzeKiss(const std::vector<int> &data) {
    //std::cout << "Inside analyzer function." << std::endl;
//define some vars
    this->inputLength = data.size();
    this->buffCount = inputLength/vecSize;
    this->buffLeftover = inputLength%vecSize;
    bool isLeftovers = false;
    vector<vector<int> > matrixoutput;
    kiss_fft_scalar inputfft[vecSize];
    memset( inputfft, 0, vecSize*sizeof(kiss_fft_scalar) );
    vector<int> debuginput;
    for (int i = 0; i < vecSize; i++) {debuginput.push_back(0);}
    kiss_fft_cpx outputfft[vecSize];
    kiss_fftr_cfg cfg;
    cfg = kiss_fftr_alloc(vecSize,0,nullptr,nullptr);
////analyze chunks periodically
    if (buffLeftover > 0) {buffCount = buffCount + 1; isLeftovers = true;} // if leftovers exist, increase buffer count by 1
    for (int i = 0; i < buffCount; i++) {

        if (i%analysisPeriod == 0) {

            if ((i+1 == buffCount) && (isLeftovers == true)) { // pad with zeros if at last chunk
                //if final chunk & leftovers exist, fill with zeros
                for (int k = 0; k < vecSize; k++) { // fill with zeros
                    inputfft[k+1] = 0;
                }
                //push leftovers into vector from the front
                for (int k = 0; k < buffLeftover; k++) { // fill beginning with leftovers
                    inputfft[k+1] = data[(i*vecSize)+1];
                }

            } else {
                //fill input buffer normally
                for (unsigned k = 0; k < vecSize; k++) {
                    unsigned long l = (i*vecSize)+1+k;
                    //cout << "Filling " << k+1 << "/" << vecSize << " with data[" << l << "] = " << data[l] << endl;
                    inputfft[k] = data[l];
                }

            }

//    for (int k = 0; k < vecSize; k++) {
//       double intcos = 2000*cos((PI/2)*k);
//       inputfft[k] = (int)(intcos);
//       debuginput[k] = (int)(intcos);
//    }
            //cout << "about to call fftr... " << endl;
//TODO: Apply window to input fft here!
//            for (int i = 0; i < vecSize; i++) {
//                double multiplier = 0.5 * (1 - cos(2*PI*i/(vecSize-1))); //hann window
//                double inputdouble = (double)(inputfft[i]);
//                inputdouble *= multiplier;
//                inputfft[i] = (int)(inputdouble);
//            }
            for (int i = 0; i < vecSize; i++) {
                debuginput[i] = inputfft[i];
            }
            signaldata.push_back(debuginput);

            kiss_fftr(cfg, inputfft, outputfft);
            kiss_fft_cleanup();
            vector<int> temp;
            for (int k = 0; k < vecSize; k++) {
                //cout << "find magnitude of " << outputfft[k+1].r << " + i" << outputfft[k+1].i << endl;
                double absmag = sqrt(pow(outputfft[k+1].r,2) + pow(outputfft[k+1].i,2));
                double dbmag = 10*log(absmag/131148); // relative is max fft output value for int input
                double noisefloor = 60;
                if(dbmag < -noisefloor) {dbmag = -noisefloor;}
                if(dbmag > 0) {dbmag = 0;}
                dbmag = (dbmag+noisefloor*1.00001)*(32000/noisefloor); // translate [-nf,0] to [0,nf] and then [0,32787]
                temp.push_back((int)(dbmag));
            }

            matrixoutput.push_back(temp);
        }
    }
float time = (float)(inputLength)/(float)(sampleRate);
float period = (float)(vecSize)*(float)(analysisPeriod)/(float)(sampleRate);
std::cout << fixed << setprecision(3);
std::cout << "Analyzed " << time << "s of audio every " << period << "s using Kiss_FFT" << endl;
std::cout << "Returning output matrix of dimensions " << matrixoutput.size() << "x" << matrixoutput[0].size() << "." << endl;
    return matrixoutput;
}

std::vector<std::vector<int> > FFTAnalyzer::fileAnalyzeKyoto(const std::vector<int> &data) {
    //std::cout << "Inside analyzer function." << std::endl;
//define some vars
    this->inputLength = data.size();
    this->buffCount = inputLength/vecSize;
    this->buffLeftover = inputLength%vecSize;
    bool isLeftovers = false;
    vector<vector<int> > matrixoutput;

    //create inputfft array using kyoto standards
    int n = vecSize;
    int nsqrt = (int)(sqrt(n));
    int ip[nsqrt + 2];
    double a[2*n + 1], w[2*n * 5 / 4];
    ip[0] = 0;

    vector<int> debuginput;
    for (int i = 0; i < vecSize; i++) {debuginput.push_back(0);}
////analyze chunks periodically
    if (buffLeftover > 0) {buffCount = buffCount + 1; isLeftovers = true;} // if leftovers exist, increase buffer count by 1
    for (int i = 0; i < buffCount; i++) {

        if (i%analysisPeriod == 0) {

            // fill a[] for analysis
            if ((i+1 == buffCount) && (isLeftovers == true)) { // pad with zeros if at last chunk
                //if final chunk & leftovers exist, fill with zeros
                for (int k = 0; k <= 2*vecSize; k++) { // fill with zeros
                    a[k] = 0;
                }
                //push leftovers into vector from the front
                for (int k = 0; k < buffLeftover; k++) { // fill beginning with leftovers
                    a[2*k] = data[(i*vecSize)+1];
                }

            } else {
                //fill with zeros to negate previous fft
                for (int k = 0; k <= 2*vecSize; k++) { // fill with zeros
                    a[k] = 0;
                }
                //fill input buffer real (even) values
                for (unsigned k = 0; k < vecSize; k++) {
                    unsigned long l = (i*vecSize)+1+k;
                    //cout << "Filling " << k+1 << "/" << vecSize << " with data[" << l << "] = " << data[l] << endl;
                    a[2*k] = data[l];
                }

            }

//    for (int k = 0; k < vecSize; k++) {
//       double intcos = 2000*cos((PI/2)*k);
//       inputfft[k] = (int)(intcos);
//       debuginput[k] = (int)(intcos);
//    }
            //cout << "about to call fftr... " << endl;
//TODO: Apply window to input fft here!
            for (int k = 0; k < vecSize; k++) {
                double multiplier = 0.5 * (1 - cos(2*PI*k/(vecSize-1))); //hann window
                double inputdouble = (double)(a[2*k]);
                inputdouble *= multiplier;
                a[2*k] = inputdouble;
            }
            for (int k = 0; k < vecSize; k++) {
                debuginput[k] = a[2*k];
            }

            // output kyoto input to file for debug
            std::remove("kyotoInput");
            std::ofstream kyotodebug;
            kyotodebug.open("kyotoInput");
            int arrLength = sizeof(a)/sizeof(*a);
            for (int k = 0; k < arrLength; k++) {
                kyotodebug << a[k] << endl;
            } kyotodebug.close();

            signaldata.push_back(debuginput);

            // call fftw function
            cdft(n, 1, a, ip, w);
            // fftw cleanup if necessary

            vector<int> temp;
            for (int k = 0; k < vecSize; k++) {
                //cout << "find magnitude of " << a[2*k] << " + i" << a[2*k+1] << endl;
                double absmag = sqrt(pow(a[2*k],2)+pow(a[2*k+1],2));
                double dbmag = 10*log(absmag/131148); // relative is max fft output value for int input
                double noisefloor = 40;
                if(dbmag < -noisefloor) {dbmag = -noisefloor;}
                if(dbmag > 0) {dbmag = 0;}
                dbmag = (dbmag+noisefloor*1.00001)*(32000/noisefloor); // translate [-nf,0] to [0,nf] and then [0,32787]
                temp.push_back((int)(absmag));
            }

            matrixoutput.push_back(temp);
        }
    }
// display metrics
float time = (float)(inputLength)/(float)(sampleRate);
float period = (float)(vecSize)*(float)(analysisPeriod)/(float)(sampleRate);
std::cout << fixed << setprecision(3);
std::cout << "Analyzed " << time << "s of audio every " << period << "s using Kyoto FFT" << endl;
std::cout << "Returning output matrix of dimensions " << matrixoutput.size() << "x" << matrixoutput[0].size() << "." << endl;
    return matrixoutput;
}
