//
//  switzerlandTest.cpp
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 09/06/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "OCREngine.h"
#include "LPStandarSegmenter.h"
#include "LPSwitzerlandSegmenter.h"
#include "gabor.h"


using namespace std;
using namespace cv;

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";
string getCountryCode(Mat inputFile, Mat** withoutStrip);


int main (int argc, char **argv){
    Mat source = imread(genFullPath("sample1_CH.jpg"));
    
    
    LPSwitzerlandSegmenter seg = LPSwitzerlandSegmenter(source);
    seg.run();
    vector<Mat> result = seg.getResult();
    
    
    OCREngine ocrEngine = OCREngine(NULL, "eng");
  //  ocrEngine.setWhiteList("0123456789");
    
    stringstream strStream;
    for (int i = 0; i < result.size(); ++i){
        cvtColor(result[i], result[i], CV_BGR2GRAY);
      //  showImageGUI("meh", 27, result[i]);
        stringstream filename;
        filename << i << ".jpg";
        filename.clear();
        stringstream fil2;
        fil2 << genFullPath(filename.str().c_str());
        imwrite(fil2.str().c_str(), result[i]);
        Mat temp; result[i].copyTo(temp); /** If I don't do this, tesseract it seems to that it try to free the image, and some weird memory exception occurs */
        ocrEngine.feedImage(temp.data, 1, temp.step1(), 0, 0, temp.cols, temp.rows);
        strStream << ocrEngine.getText();
        ocrEngine.clean();
    }
    
    string output = strStream.str();
    
    output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());
    
    cout << "Output: " << output << endl;
    
    return 0;
}



string genFullPath(const char file[]){
    stringstream fullpath;
    
    fullpath << BASEPATH << file;
    
    return fullpath.str();
}

void showImageGUI(const String sWindowName, int iStopKey, const Mat oInputImage){
    
    namedWindow(sWindowName);
    imshow(sWindowName, oInputImage);
    while (true){
        if (waitKey() == iStopKey)
            break;
    }
    destroyWindow(sWindowName);
}