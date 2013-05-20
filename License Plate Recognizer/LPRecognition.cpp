//
//  LPRecognition.cpp
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 20/05/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "LPEuroCountryExtract.h"
#include "OCREngine.h"


using namespace std;
using namespace cv;

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);
string getCountryCode(Mat inputFile, Mat** withoutStrip);


int main(int argc, char**argv){
    
    Mat inputfile = imread(genFullPath("sample2.jpg"));

    Mat** withoutStrip = new Mat*[1]; // Si creo memoria con malloc, luego ocrEngine falla O_O!!!!!!!!!!
    string countryCode = getCountryCode(inputfile, withoutStrip);
    showImageGUI("WithoutStrip", 27, *(*withoutStrip));
    cout << "Country code: " << countryCode << endl;
    
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
        showImageGUI("test", 27, listChars[i]);
        cvtColor(listChars[i], listChars[i], CV_BGR2GRAY);
        ocrEngine.feedImage(listChars[i].data, 1, listChars[i].step1(), 0, 0, listChars[i].cols, listChars[i].rows);
        strStream << ocrEngine.getText();
       // ocrEngine.clean();
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