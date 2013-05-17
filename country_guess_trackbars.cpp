//
//  country_guess.cpp
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 18/04/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#include <iostream>
#include "OCREngine.h"
#include "Segmenter.h"
#include "FilterType.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);

int minH = 89, minS = 138, minV = 51, maxH = 121, maxS = 255, maxV = 255;
Mat blueZone;
Mat hsvImage;

void on_trackbar(int, void*){
    inRange(hsvImage, Scalar(minH,minS,minV), Scalar(maxH,maxS,maxV), blueZone);

    imshow("blueZone", blueZone);
    
    printf("Min threshold: HSV:(%d,%d,%d)\n",minH,minS,minV);
    printf("Max threshold: HSV:(%d,%d,%d)\n",maxH,maxS,maxV);

}



int main (int argc, char **argv){
    
    
    Mat source = imread(genFullPath("sample9.jpg"));
    cvtColor(source, hsvImage, CV_BGR2HSV);
    blueZone = Mat(source.rows, source.cols, hsvImage.type());
    
    
    namedWindow("blueZone");
    createTrackbar("minH", "blueZone", &minH, 255, on_trackbar);
    createTrackbar("minS", "blueZone", &minS, 255, on_trackbar);
    createTrackbar("minV", "blueZone", &minV, 255, on_trackbar);

    createTrackbar("maxH", "blueZone", &maxH, 255, on_trackbar);
    createTrackbar("maxS", "blueZone", &maxS, 255, on_trackbar);
    createTrackbar("maxV", "blueZone", &maxV, 255, on_trackbar);

    on_trackbar(0, 0);
    
    waitKey(0);
    
    //showImageGUI("blueZone", 27, blueZone);
    
    
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