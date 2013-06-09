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
#include "gabor.h"
#include "Rectangle.h"



using namespace std;
using namespace cv;

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";

string genFullPath(const char file[]);
void showImageGUI(const string sWindowName, int iStopKey, const Mat oInputImage);
string getCountryCode(Mat inputFile, Mat** withoutStrip);
void doTrackbar(int v, void* p);

bool squareCompare (Rectangle i, Rectangle j){
    return (i.getCenter().x < j.getCenter().x);
}


/** GLOBAL VARIABLES */
Mat withoutStrip;
int thresholdVariable;
int squareRatioInt;
int squareRatioDecimal;

double minSquareHeight;
double minSquareWidth;

int maxAllowedHeightVariation;

int main(int argc, char**argv){
    
    Mat inputfile = imread(genFullPath("russiaTest1.jpg"));
    Mat originalInputFile; inputfile.copyTo(originalInputFile);
    Mat** withoutStripPtr = new Mat*[1]; // Si creo memoria con malloc, luego ocrEngine falla O_O!!!!!!!!!!
    
    string countryCode = getCountryCode(inputfile, withoutStripPtr);
   // showImageGUI("WithoutStrip", 27, *(*withoutStripPtr));
    
    
    
    thresholdVariable = 151;
    
    squareRatioInt = 3;
    squareRatioDecimal = 20;
   
    minSquareWidth = 5; // Percentage
    minSquareHeight = 14.4; // Percentage;
    maxAllowedHeightVariation = 7;
    
    if (countryCode.compare("") != 0){
        withoutStrip = *(*withoutStripPtr);
        cout << "Country code: " << countryCode << endl;
    }
    else{
        cout << "Country code: XX (NO CC)" << endl;
        withoutStrip = originalInputFile;
    }
    
    
    namedWindow("result");
    namedWindow("threshold");
    namedWindow("contourDraw");
    
    doTrackbar(0, 0);
    waitKey(0);
    
    return 0;
        
}


void doTrackbar(int v, void* p){
    cout << "Current threshold value: " << thresholdVariable << endl;
    /**********************************/
    /******* INPUT ********************/
    /**********************************/
    Mat input = Mat(); withoutStrip.copyTo(input);
    cvtColor(input, input, CV_BGR2GRAY);
    
    
    /**********************************/
    /******* FILTER ********************/
    /**********************************/
    Mat kernel = mkKernel(21,1,95,0.69,21);
    input = processGabor(input,kernel,21);
    
    /**********************************/
    /******* THRESHOLD ********************/
    /**********************************/
    threshold(input, input, thresholdVariable, 255, CV_THRESH_BINARY);

    Mat threshold = Mat();
    input.copyTo(threshold);
    imshow("threshold",threshold);
    /**********************************/
    /******* CONTOURS ********************/
    /**********************************/
    vector<vector<Point> > _contours;
    vector<Vec4i> _contourHierarchy;
    vector<Rectangle> _segmentedSquares;
    Mat contourInputTemp;
    input.convertTo(contourInputTemp, CV_8UC1);

    findContours(contourInputTemp, _contours, _contourHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS);
    
    Mat contourDraw = Mat::zeros(contourInputTemp.size(), CV_8UC3);
    RNG rng(12345);
    for (int i = 0 ; i < _contours.size(); ++i){
        Scalar color = Scalar(rng.uniform(0, 255),rng.uniform(0,255), rng.uniform(0, 255));
        drawContours(contourDraw, _contours, i, color);
    }
    imshow("contourDraw", contourDraw);
    
    /** Get contours squares */
    double squareRatioRes = (double)squareRatioInt;
    squareRatioRes += (squareRatioDecimal/100.0);
    printf("CurrentSquareRatio: %lf\n",squareRatioRes);
    
    cout << "Square width: " << minSquareWidth << "/" << input.cols <<  "(" << (minSquareWidth*100)/input.cols << "%)" << endl;
    cout << "Square height: " << minSquareHeight << "/" << input.rows <<  "(" << (minSquareHeight*100)/input.rows << "%)" << endl;
    
    vector<Rectangle> listOfSquares;
    double maxHeight = 0;
    int maxXcoordinate = 0;
    for (int i = 0 ; i < _contours.size(); ++i){
        Rect square = boundingRect(_contours[i]);
        // FILTER SOMEHOW
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
        
        /**
         *  Three constraints, the ratio of the square must be less than the max allowed square ratio
         *                   , the height must represent a certain percentage of the whole heigth 14.4%
         *                   , the width must represent a certain percentage of the whole width 5%
         */
        if ((f1/f2) < squareRatioRes && ((height*100)/input.rows) > minSquareHeight && ((width*100)/input.cols) > minSquareWidth){
           
            if (maxHeight < height){
                maxHeight = height;
                maxXcoordinate = square.y;
            }

        
            Moments mnt = moments(_contours[i]);
            double xCentroid = mnt.m10/mnt.m00;
            double yCentroid = mnt.m01/mnt.m00;
        
            Point center; center.x = xCentroid; center.y = yCentroid;
        
            Rectangle rectangle = Rectangle(square,center);
            listOfSquares.push_back(rectangle);
        }
    }
    
    /** Sort them */
    sort(listOfSquares.begin(),listOfSquares.end(),squareCompare);
    
    /** Get original for drawing */
    Mat forDraw; withoutStrip.copyTo(forDraw);
    /** Draw rectangles in image */
    cout << "\t Drawing Rectangles..." << endl;
    for (int i = 0; i < listOfSquares.size(); ++i){
        cout << "\t Heigth (" << i << ") = " << listOfSquares[i].getRect().height << endl;
        Rect squareGot = listOfSquares[i].getRect();
        
        /** Post filtering!!!!, the height of the square must be greather than the half of the tallest square found */
        /* This case occurs when we have Ä Ö Ü */
        // MAX_ALLOWED_HEIGHT_VARIATION = 5
        if (squareGot.height >= (maxHeight)/2){

   
            if (abs(maxHeight-squareGot.height) > maxAllowedHeightVariation){
                // EXPAND SQUARE
                int idxToGet;
                if (i < listOfSquares.size() - 1){
                    // I can get the next one
                    idxToGet = i + 1;
                }
                else if (i == 0){
                    // Get the second one
                    idxToGet = 1;
                }
                else{
                    // Otherwise get the previous square
                    idxToGet = i - 1;
                }
            
                cout << "Expand " << endl;
                squareGot.y = listOfSquares[idxToGet].getRect().y - 4;
                squareGot.height = listOfSquares[idxToGet].getRect().height + 4;
            }
        
            rectangle(forDraw, squareGot,Scalar(0,0,255));
        }
    }

    // Show the current result 
    imshow("result", forDraw);
    
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