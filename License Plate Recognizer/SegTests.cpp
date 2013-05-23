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
#include "LPStandarSegmenter.h"
#include "LPGermanySegmenter.h"

using namespace std;
using namespace cv;

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";
string getCountryCode(Mat inputFile, Mat** withoutStrip);


int main (int argc, char **argv){
    Mat source = imread(genFullPath("sample1.jpg"));
    
    LPGermanySegmenter seg = LPGermanySegmenter(source);
    seg.run();
    vector<Mat> result = seg.getResult();
   
    for (int i = 0 ; i < result.size(); ++i){
        showImageGUI("char", 27, result[i]);
    }
    
    return 0;
}


string getCountryCode(Mat inputFile, Mat** withoutStrip){
    Mat temp = Mat(inputFile.rows,inputFile.cols,inputFile.type());
    inputFile.copyTo(temp);
    LPEuroCountryExtract countryExtract = LPEuroCountryExtract(&temp);
    OCREngine ocrEngine = OCREngine(NULL, "blueBand");
    
    vector<Mat> listChars = countryExtract.getCharacters();
    
    stringstream strStream;
    for (int i = 0; i < listChars.size(); ++i){
        //   showImageGUI("test", 27, listChars[i]);
        cvtColor(listChars[i], listChars[i], CV_BGR2GRAY);
        Mat temp; listChars[i].copyTo(temp); /** If I don't do this, tesseract it seems to that it try to free the image, and some weird memory exception occurs */
        ocrEngine.feedImage(temp.data, 1, temp.step1(), 0, 0, temp.cols, temp.rows);
        strStream << ocrEngine.getText();
        ocrEngine.clean();
    }
    
    *withoutStrip = countryExtract.getCroppedWithoutStrip();
    
    string output = strStream.str();
    
    output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());
    
    return output;
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