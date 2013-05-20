//
//  country_test.cpp
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 27/04/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "OCREngine.h"
#include "LPEuroCountryExtract.h"

using namespace std;
using namespace cv;

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";


int main (int argc, char **argv){
    Mat source = imread(genFullPath("ro.jpg"));

    clock_t begin = clock();
    
    LPEuroCountryExtract countryExtract = LPEuroCountryExtract(&source);
    
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
    cout << "Time elapsed: "  << elapsed_secs*1000 << " ms." << endl;
    
    OCREngine ocrEngine = OCREngine(NULL, "blueBand");

    vector<Mat> listChars = countryExtract.getCharacters();
    
    stringstream strStream;
    for (int i = 0; i < listChars.size(); ++i){
        showImageGUI("test", 27, listChars[i]);
        cvtColor(listChars[i], listChars[i], CV_BGR2GRAY);
        ocrEngine.feedImage(listChars[i].data, 1, listChars[i].step1(), 0, 0, listChars[i].cols, listChars[i].rows);
        strStream << ocrEngine.getText();
        
        ocrEngine.clean();
    }
    
    string output = strStream.str();
    
    output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());


    cout << "Country Code: " << output << endl;

    
    return 0;
}


void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage){
    
    namedWindow(sWindowName);
    imshow(sWindowName, oInputImage);
    while (true){
        if (waitKey() == iStopKey)
            break;
    }
    destroyWindow(sWindowName);
}

string genFullPath(const char file[]){
    stringstream fullpath;
    
    fullpath << BASEPATH << file;
    
    return fullpath.str();
}