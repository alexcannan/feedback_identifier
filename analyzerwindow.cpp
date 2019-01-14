#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "src/FileReader.h"
#include "src/FeedbackID.h"
#include "src/FFTAnalyzer.h"
#include "src/FBplotter.h"
#include "src/AudioFile.h"
#include <typeinfo>
#include "analyzerwindow.h"
#include "ui_analyzerwindow.h"
#include <QFileDialog>
#include <QMessageBox>



AnalyzerWindow::AnalyzerWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::AnalyzerWindow) {
    ui->setupUi(this);
    std::string path = QCoreApplication::applicationDirPath().toLocal8Bit().constData();;
    std::cout << path << endl;
}

AnalyzerWindow::~AnalyzerWindow(){
    delete ui;
}


void AnalyzerWindow::on_QuitButton_clicked(){
    AnalyzerWindow::close();
}

void AnalyzerWindow::on_AnalyzeButton_clicked()
{
    FileReader reader;

    std::string utf8_text = filename.toUtf8().constData();
    std::cout << "Relative Filename: " << utf8_text << std::endl;

ui->progressBar->setValue(20);

// Take data from wav file and input into samples... not working? Getting values close to zero randomly throughout data vector.
//    vector<int> data = reader.read(utf8_text);

////Check for errors in file reading
//    if (data.size() < 10 && data.size() > 0) { // returned error vector if size less than 10
//        QString errors = "";
//        for (int i = 0; i < data.size(); i++) {
//            if (data[i] == 1) {errors += "ec1: File not found\n";}
//            if (data[i] == 2) {errors += "ec2: File too small to be .wav\n";}
//            if (data[i] == 3) {errors += "ec3: Header mismatch 1. May not be .wav file\n";}
//            if (data[i] == 4) {errors += "ec4: Header mismatch 2. May not be .wav file\n";}
//            if (data[i] == 5) {errors += "ec5: File must be uncompressed\n";}
//            if (data[i] == 6) {errors += "ec6: File must be mono\n";}
//            if (data[i] == 7) {errors += "ec7: Header mismatch 3. May not be .wav file\n";}
//        }
//        errors += "\nAborting process.";
//        QMessageBox Error;
//        Error.setText(errors);
//        Error.setIcon(QMessageBox::Warning);
//        Error.setWindowTitle("Error");
//        Error.exec();
//        ui->progressBar->setValue(0);

//        return;
//    }
//    if (data.size() == 0) {std::cout << "Returned empty vector. What's up with that?" << std::endl; return;}

// Trying AudioFile library, found on github. See AudioFile.h for info

    vector<int> data;
    AudioFile<double> af;
    af.load(utf8_text);
    int numSamples = af.getNumSamplesPerChannel();
    for (int i = 0; i < numSamples; i++) {
        double temp = af.samples[0][i];
        data.push_back((int)(temp*32767));
    }

    // output to file for debug
    remove("wavOutput.csv");
    ofstream fileread;
    fileread.open("wavOutput.csv");
    for (unsigned i = 0; i < data.size(); i++) {
        fileread << data[i] <<  ", " << af.samples[0][i] << endl;
    } fileread.close();

//Take FFT Periodically

    unsigned guiAnalysisPeriod = ui->analysisPeriodSpinner->value();
    unsigned fftSize;
    if (ui->radio512->isChecked()) {fftSize = 512;}
    if (ui->radio1024->isChecked()) {fftSize = 1024;}
    if (ui->radio2048->isChecked()) {fftSize = 2048;}
    if (ui->radio4096->isChecked()) {fftSize = 4096;}
    if (ui->radioKyoto->isChecked()) {fftSize = fftSize*2;}
    FFTAnalyzer FFTtest(fftSize, guiAnalysisPeriod, af.getSampleRate());
    vector<vector<int> > fftOut;
    bool isKyoto;
    if (ui->radioKiss->isChecked()) {fftOut = FFTtest.fileAnalyzeKiss(data); isKyoto = false;}
    if (ui->radioKyoto->isChecked()) {fftOut = FFTtest.fileAnalyzeKyoto(data); isKyoto = true;}

ui->progressBar->setValue(40);

    FeedbackID IDtest(fftOut);

    IDtest.setSNLWeight(ui->sliderSNR->value());
    IDtest.setSwellWeight(ui->sliderSwell->value());
    IDtest.setHarmonicWeight(ui->sliderHarmonics->value());
    IDtest.setSpecWeight(ui->sliderSpecWidth->value());
    IDtest.setSustainWeight(ui->sliderSustain->value());

    vector<vector<int> > FBProbs = IDtest.findFeedback();

ui->progressBar->setValue(70);

//output analysis matricies to debug
std::ofstream m2plot;
std::ofstream m2analyze;
m2plot.open("fftmatrix.csv");
m2analyze.open("signaldata.csv");
for (int i = 0; i < fftOut.size(); i++) {
    for (int j = 0; j < fftOut[1].size()-1; j++){
        m2plot << fftOut[i][j] << ", ";
        m2analyze << FFTtest.signaldata[i][j] << ", ";
    }
    m2plot << fftOut[i][1023] << std::endl;
    m2analyze << FFTtest.signaldata[i][1023] << std::endl;
} m2plot.close(); m2analyze.close();

    FBPlotter fbp(FBProbs, fftOut);
    fbp.RGBEncode("rgbdata",FBProbs,fftOut);
    fbp.Plot("rgbdata", isKyoto);




ui->progressBar->setValue(100);
}

void AnalyzerWindow::on_pushBrowse_clicked()
{
    filename = QFileDialog::getOpenFileName(this,tr("Open file"),"../feedback_identifier/wavfiles","Wav files (*.wav)");
    ui->filenameEdit->setText(filename);
}

void AnalyzerWindow::on_analysisperButton_clicked()
{
    QMessageBox info;
    info.setText("The analysis period determines how often the signal is sampled. Note that the signal is split into 1024 point buffers already. An analysis period of 5 indicates that only every 5th buffer is analyzed; An analysis period of 3 means every 3rd buffer; and so on.");
    info.setIcon(QMessageBox::Question);
    info.setWindowTitle("Analysis Period Information");
    info.exec();
}

void AnalyzerWindow::on_sustainButton_clicked()
{
    QMessageBox info;
    info.setText("The Sustain check generates probabilities based on how frequencies are sustained over time. It is meant to catch frequencies that are already feeding back significantly and are saturated.");
    info.setIcon(QMessageBox::Question);
    info.setWindowTitle("Sustain Check Information");
    info.exec();
}

void AnalyzerWindow::on_specwidthButton_clicked()
{
    QMessageBox info;
    info.setText("This check looks at neighboring frequencies to see if they are also increasing. The more neighboring frequencies that are increasing, the higher probability that this frequency is also feedback.");
    info.setIcon(QMessageBox::Question);
    info.setWindowTitle("Spectral Width Check Information");
    info.exec();
}

void AnalyzerWindow::on_harmonicWeight_clicked()
{
    QMessageBox info;
    info.setText("The Harmonic check looks at the harmonics of frequencies that are determined to be feeding back. Budding feedback will not have harmonic content, so this allows us to distinguish between a musical swell and budding feedback.");
    info.setIcon(QMessageBox::Question);
    info.setWindowTitle("Harmonic Check Information");
    info.exec();
}

void AnalyzerWindow::on_swellButton_clicked()
{
    QMessageBox info;
    info.setText("This check increases the probability of feedback as the magnitude of its frequency band increases. In other words, if the band is getting louder, its more probably feedback.");
    info.setIcon(QMessageBox::Question);
    info.setWindowTitle("Swell Check Information");
    info.exec();
}

void AnalyzerWindow::on_snrButton_clicked()
{
    QMessageBox info;
    info.setText("This check sets the feedback probability of frequency bands below a certain threshold (the noise floor) to zero. At the same time, if a frequency band has an incredibly high peak, it sets the probability as 1.");
    info.setIcon(QMessageBox::Question);
    info.setWindowTitle("SNR Check Information");
    info.exec();
}
