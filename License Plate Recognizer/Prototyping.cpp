//
//  prototyping.cpp
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
#include "LPRussiaSegmenter.h"
#include "gabor.h"

using namespace std;
using namespace cv;

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";
string getCountryCode(Mat inputFile, Mat** withoutStrip);
void Process(int , void *);


Mat *original;
int THRESHOLD_BINARIZE = 92;
int SQUARE_RATIO_INT = 2;
int SQUARE_RATIO_DEC = 50;
int MIN_SQUARE_HEIGHT_PERCENTAGE_INT = 18;
int MIN_SQUARE_HEIGHT_PERCENTAGE_DEC = 18;

int MIN_SQUARE_WIDTH_PERCENTAGE_INT = 4;
int MIN_SQUARE_WIDTH_PERCENTAGE_DEC = 0;

int main (int argc, char **argv){
    Mat source = imread(genFullPath("swisszurich.jpg"));
    original = new Mat();
    source.copyTo(*original);
    
    
    
    namedWindow("Processing");
    createTrackbar("SQR_INT", "Processing", &SQUARE_RATIO_INT, 255, Process);
    createTrackbar("SQR_DEC", "Processing", &SQUARE_RATIO_DEC, 255, Process);
    
    createTrackbar("SQR_HEIGHT_INT", "Processing", &MIN_SQUARE_HEIGHT_PERCENTAGE_INT, 255, Process);
    createTrackbar("SQR_HEIGHT_DEC", "Processing", &MIN_SQUARE_HEIGHT_PERCENTAGE_DEC, 255, Process);
    
    
    createTrackbar("SQR_WIDTH_INT", "Processing", &MIN_SQUARE_WIDTH_PERCENTAGE_INT, 255, Process);
    createTrackbar("SQR_WIDTH_DEC", "Processing", &MIN_SQUARE_WIDTH_PERCENTAGE_DEC, 255, Process);
    
    
    Process(0, 0);
    
    waitKey(0);
    
    
    return 0;
}

bool squareCompare (Rectangle i, Rectangle j){
    return (i.getCenter().x < j.getCenter().x);
}

bool descendingCompare(vector<Point> i, vector<Point> j){
    return i.size() > j.size();
}

void Process(int , void *){
    Mat *input = new Mat();
    original->copyTo(*input);
    
    cvtColor(*input, *input, CV_BGR2GRAY);
    
    Mat kernel = mkKernel(21,1,95,0.69,21);
    *input = processGabor(*input,kernel,21);
    
    threshold(*input, *input, THRESHOLD_BINARIZE, 255, CV_THRESH_BINARY);
    
    vector<vector<Point> > _contours;
    vector<Vec4i> _contourHierarchy;
    
    Mat contourTemp; input->convertTo(contourTemp,CV_8UC1);
    findContours(contourTemp, _contours, _contourHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS);
    
    
    Mat toDraw = Mat();
    original->copyTo(toDraw);
    vector<Rectangle> listOfRectangles;
    std::sort(_contours.begin(), _contours.end(), descendingCompare);
    
    double maxHeight = 0;
    int maxXcoordinate = 0;
    
    for (int i = 0; i<_contours.size(); ++i) {
        Rect square = boundingRect(_contours[i]);
        
        // FILTER
        double height, width;
        height = square.height;
        width = square.width;
        
        double f1,f2;
        /** We need in f1 the biggest, for avoiding problems by comparing with the RATIO THRESHOLD */
        if (height > width){
            f1 = height;
            f2 = width;
        }
        else{
            f1 = width;
            f2 = height;
        }
        
        double SQUARE_RATIO;
        double MIN_SQUARE_HEIGHT_PERCENTAGE;
        double MIN_SQUARE_WIDTH_PERCENTAGE;
        
        MIN_SQUARE_HEIGHT_PERCENTAGE = MIN_SQUARE_HEIGHT_PERCENTAGE_INT + (MIN_SQUARE_HEIGHT_PERCENTAGE_DEC/100.0);
        
        MIN_SQUARE_WIDTH_PERCENTAGE = MIN_SQUARE_WIDTH_PERCENTAGE_INT + (MIN_SQUARE_WIDTH_PERCENTAGE_DEC/100.0);
        
        SQUARE_RATIO = SQUARE_RATIO_INT + (SQUARE_RATIO_DEC/100.0);
        
        
        cout << "CurrentSquareRatio: " << SQUARE_RATIO << endl;
        cout << "Current MIN_SQUARE_HEIGHT_PERCENTAGE: " << MIN_SQUARE_HEIGHT_PERCENTAGE << endl;
        
        cout << "Current MIN_SQUARE_WIDTH_PERCENTAGE: " << MIN_SQUARE_WIDTH_PERCENTAGE << endl;
        
        if( (f1/f2) < SQUARE_RATIO && ((height*100)/input->rows)> MIN_SQUARE_HEIGHT_PERCENTAGE && ((width*100)/input->cols) > MIN_SQUARE_WIDTH_PERCENTAGE){
            
            if (maxHeight < height){
                maxHeight = height;
                maxXcoordinate = square.y;
            }
            
            
            Moments mnt = moments(_contours[i]);
            double xCentroid = mnt.m10/mnt.m00;
            double yCentroid = mnt.m01/mnt.m00;
            Point center; center.x = xCentroid; center.y = yCentroid;
            
            Rectangle rectangle = Rectangle(square,center);
            listOfRectangles.push_back(rectangle);
        }
    }
    
    sort(listOfRectangles.begin(),listOfRectangles.end(),squareCompare);
    
    vector<Mat> resultToReturn;
    for (int i = 0; i < listOfRectangles.size(); ++i){
        
        Rect squareGot = listOfRectangles[i].getRect();
        
        /** Post filtering!!!!, the height of the square must be greather than the half of the tallest square found */
        /* This case occurs when we have Ä Ö Ü */
        // MAX_ALLOWED_HEIGHT_VARIATION = 7
        if (squareGot.height >= (maxHeight)/2){
            rectangle(toDraw, listOfRectangles[i].getRect(), Scalar(0,255,0));
            
        }
    }
    
    
    imshow("Processing", toDraw);
    
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