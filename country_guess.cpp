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
#include <vector>
#include "FilterType.h"
#include "gabor.h"

using namespace std;

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);

int minH = 89, minS = 138, minV = 51, maxH = 121, maxS = 255, maxV = 255;
Mat blueZone;
Mat hsvImage;

bool DescendingCompare (std::vector<cv::Point> i,std::vector<cv::Point> j) { return (i.size()>j.size()); }
int thresholdV = 92;
Mat original_track;
Mat converted;
vector<vector<Point> > contoursBlue;
vector<Vec4i> contourHierarchyBlue;
Mat copyExtracted;
OCREngine ocrEngine = OCREngine(NULL, "blueBand");

// Threshold Value 92, (experimental value) !!!!!!


void do_track(int , void*){
    threshold(original_track,converted, thresholdV ,255,CV_THRESH_BINARY);
    
    
    imshow("track", converted);

    findContours(converted, contoursBlue, contourHierarchyBlue, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    sort(contoursBlue.begin(), contoursBlue.end(), DescendingCompare);
    
    std::vector<std::vector<cv::Point> > contours_poly(1);
    
    if (contoursBlue.size() != 0)
    // Determine an approximate polygon for v[0] which is the largest contour
        approxPolyDP( cv::Mat(contoursBlue[0]), contours_poly[0], 3, true );
    
    Mat squareDraw; copyExtracted.copyTo(squareDraw);
    Rect squareLetter1;

    if (contoursBlue.size() != 0)
        squareLetter1 = boundingRect(contours_poly[0]);
     
     squareLetter1.x -= 3;
     if (squareLetter1.x < 0)
     squareLetter1.x = 0;
     squareLetter1.y -= 3;
     if (squareLetter1.y < 0)
     squareLetter1.y = 0;
     squareLetter1.width += 6;
     squareLetter1.height += 6;
     
    
    if ( (squareLetter1.x + squareLetter1.width ) > copyExtracted.cols)
        squareLetter1.width = copyExtracted.cols - squareLetter1.x;
    
    if ( (squareLetter1.y + squareLetter1.height) > copyExtracted.rows)
        squareLetter1.height = copyExtracted.rows - squareLetter1.y;

    
     rectangle(squareDraw, squareLetter1, Scalar(0,255,0));
    
    Mat letter1 = copyExtracted(squareLetter1);
    
   // if (contoursBlue.size() != 0)
     //   approxPolyDP( cv::Mat(contoursBlue[1]), contours_poly[0], 3, false );
    
    //Rect squareLetter2 = boundingRect(contours_poly[0]);
    
    
 /*   squareLetter2.x -= 3;
    if (squareLetter2.x < 0)
        squareLetter2.x = 0;
    squareLetter2.y -= 3;
    if (squareLetter2.y < 0)
        squareLetter2.y = 0;
    squareLetter2.width += 6;
    squareLetter2.height += 6;
    
    if ( (squareLetter2.x + squareLetter2.width ) > copyExtracted.cols)
        squareLetter2.width = copyExtracted.cols - squareLetter2.x;
    
    if ( (squareLetter2.y + squareLetter2.height) > copyExtracted.rows)
        squareLetter2.height = copyExtracted.rows - squareLetter2.y;
    */

  //  rectangle(squareDraw, squareLetter2, Scalar(0,255,0));
//    Mat letter2 = copyExtracted(squareLetter2);

    
    
    Mat temp; copyExtracted.copyTo(temp);
    Mat rectanglesDrawing; copyExtracted.copyTo(rectanglesDrawing);

    for (int i = 0 ; i < contoursBlue.size(); ++i){
        drawContours(temp, contoursBlue, i, Scalar(0,255,0));
        Rect square = boundingRect(contoursBlue[i]);
     
        rectangle(rectanglesDrawing, square, Scalar(0,255,0));
    }
    
    // Letters has to be converted to gray
    
    cvtColor(letter1, letter1, CV_BGR2GRAY);
    
 //   ocrEngine.feedImage(letter1.data, 1, letter1.step1(), 0, 0, letter1.cols, letter1.rows);
    
   // cout << "First Letter: " << ocrEngine.getText() << endl;
    
    //ocrEngine.clean();
    
    
  //  cvtColor(letter2, letter2, CV_BGR2GRAY);

    
//    ocrEngine.feedImage(letter2.data, 1, letter2.step1(), 0, 0, letter2.cols, letter2.rows);

//    cout << "Second Letter: " << ocrEngine.getText() << endl;

    
    imshow("squareDraw", squareDraw);
    imshow("letter1", letter1);
 //   imshow("letter2", letter2);

    imshow("contoursFinal",temp);
    imshow("squares", rectanglesDrawing);
    
    cout << "Threshold value: " << thresholdV << endl;
}

int main (int argc, char **argv){
    
    ocrEngine.setSingleCharacterMode();
    ocrEngine.setWhiteList("ACDFIORSZ");
    
    Mat source = imread(genFullPath("spain.jpg"));
    cvtColor(source, hsvImage, CV_BGR2HSV);
    blueZone = Mat(source.rows, source.cols, hsvImage.type());
    
    inRange(hsvImage, Scalar(minH,minS,minV), Scalar(maxH,maxS,maxV), blueZone);

    vector<vector<Point> > contours;
    vector<Vec4i> contourHierarchy;
    
    findContours(blueZone, contours, contourHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    std::sort(contours.begin(), contours.end(),DescendingCompare);

    std::vector<std::vector<cv::Point> > contours_poly(1);
    
    // Determine an approximate polygon for v[0] which is the largest contour
    approxPolyDP( cv::Mat(contours[0]), contours_poly[0], 3, true );
    
    Rect square = boundingRect(contours_poly[0]);
    
    Mat extracted = source(square);
     extracted.copyTo(copyExtracted);
   // drawContours( source, contours_poly, 0, Scalar(0,255,0), 2, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
  

    // Adapt colour space for findContours

    cvtColor(extracted, extracted, CV_HSV2BGR);
    cvtColor(extracted, extracted, CV_BGR2GRAY);


    
    showImageGUI("extracted", 27, extracted);

  
    Mat converted;
    Mat kernel = mkKernel(7, 1, 95, 0.69, 21);
    converted = processGabor(extracted,kernel,21);
    
    showImageGUI("convertedbefore", 27, converted);

    
    converted.convertTo(converted, CV_8UC1);
    
    showImageGUI("convertedafter", 27, converted);


    
    dilate(converted, converted, NULL);
    dilate(converted, converted, NULL);
    dilate(converted, converted, NULL);
    
    converted.copyTo(original_track);
    
    namedWindow("track");
    createTrackbar("thresholdValue", "track", &thresholdV, 255, do_track);
    namedWindow("contoursFinal");
    namedWindow("squares");
    namedWindow("letter1");
    namedWindow("letter2");

    do_track(0, 0);
    
    waitKey(0);
    destroyWindow("track");
    destroyWindow("contoursFinal");

    destroyWindow("squares");
    destroyWindow("letter1");
    destroyWindow("letter2");


    
    
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