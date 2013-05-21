//
//  Segmenter.cpp
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 06/04/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#include "Segmenter.h"
#include "gabor.h"
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <iostream>
#include <limits>
#include "FilterType.h"

using namespace cv;
using namespace std;

bool squareCompare (Rectangle i, Rectangle j){
    return (i.getCenter().x < j.getCenter().x);
}

Segmenter::Segmenter(const Mat* v){

    _input = Mat();
    cvtColor(*v, _input, CV_BGR2GRAY);
    _input.copyTo(_original);

    _input.convertTo(_input, CV_32F, 1.0/255, 0);
   // _input.convertTo(_input, CV_32F);

    _filterApplied = false;
    _debug = false;
    _isDone = false;
    _characterIdx = 0;
}


void Segmenter::applyFilter(filterType f){
    switch (f) {
        case GaborFilter:
        {
            // Gabor call
            Mat kernel = mkKernel(21, 1, 95, 0.69, 21);
            _input = processGabor(_input,kernel,21);
            _filterApplied = true;
            break;
        }
        case CannyFilter:
        {
            // Canny call
            _filterApplied = false;
            cout << "Error: filter not implemented" << endl;
            break;
        }
        default:
        {
            cout << "Error: no filter applied" << endl;
            break;
        }
    }
}

/** Optional */
void Segmenter::preprocess(double thresholdValue){
    
    _input.convertTo(_input, CV_8UC1);
    threshold(_input, _input, thresholdValue, 255, CV_THRESH_BINARY);

    dilate(_input, _input, NULL);
 //   dilate(_input, _input, NULL);

}

void Segmenter::computeContours(){
    
    _input.convertTo(_input, CV_8UC1);
    
    findContours(_input, _contours, _contourHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    _contoursPresent = true;
}

void Segmenter::showContours(){
    Mat drawing = Mat::zeros( _input.size(), CV_8UC3 );
    RNG rng(12345);

    for( int i = 0; i< _contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, _contours, i, color, 2, CV_AA, _contourHierarchy, 0, Point() );
    }

    namedWindow("Contours");
    imshow("Contours", drawing);
    while (true){
        if (waitKey() == 27)
            break;
    }
    destroyWindow("Contours");
}

void Segmenter::showCurrentImage(const char* windowName){
    showWindow(windowName, _input);
}

void Segmenter::showWindow(const char* windowName, Mat image){
    namedWindow(windowName);
    imshow(windowName, image);
    while (true){
        if (waitKey() == 27)
            break;
    }
    destroyWindow(windowName);
}


void Segmenter::enableDebug(){
    _debug = true;
}
void Segmenter::disableDebug(){
    _debug = false;
}


void Segmenter::segmentFromContours(){
    if (_contoursPresent){

        
        minWidth = INFINITY;
        int squareHeightFactor = 3;
        
        for( int i = 0; i< _contours.size(); i++ )
        {
            Rect square = boundingRect(_contours[i]);
            /*if (_debug)
                rectangle(_input, square, Scalar(0,0,255));
            */
            if (square.height > _input.rows/squareHeightFactor){
                
                if (minWidth > square.width){
                    minWidth = square.width;
                }
                
                
                Moments mnt = moments(_contours[i]);
                
                double xCentroid = mnt.m10/mnt.m00;
                double yCentroid = mnt.m01/mnt.m00;
                
                Point center; center.x = xCentroid; center.y = yCentroid;
                
                Rectangle rectangle = Rectangle(square, center);
                _segmentedSquares.push_back(rectangle);
                
            }

        }
    
        sort(_segmentedSquares.begin(), _segmentedSquares.end(), squareCompare);

    }
    else{
        cout << "Error: Contours not present" << endl;
    }
}

void Segmenter::viewCandidateSquares(Mat *dest){

    Mat output;
    
    if (dest == NULL);
        _input.copyTo(output);
    
    output = *dest;
    
    for (int i = 0 ; i < _segmentedSquares.size(); ++i){
        if (_segmentedSquares[i].getRect().width >= minWidth && _segmentedSquares[i].getRect().width < minWidth*8){
            rectangle(output, _segmentedSquares[i].getRect(), Scalar(0,0,255));

        }
    }
    
    showWindow("Candidate squares", output);
}


Mat* Segmenter::getNextCharacter(){
   
    Mat* character = new Mat();
    bool charFound = false;
    
    while (!charFound){
        if (_characterIdx >= _segmentedSquares.size()){
            _isDone = true;
            _characterIdx = 0; // Reset
            return NULL;
        }
        else if (_segmentedSquares[_characterIdx].getRect().width >= minWidth && _segmentedSquares[_characterIdx].getRect().width < minWidth*8){
            Rect currentSquare = _segmentedSquares[_characterIdx].getRect();
            /** Lets 'zoom out' a bit the square **/
         /*   currentSquare.x -= 3;
            if (currentSquare.x < 0)
                currentSquare.x = 0;
            currentSquare.y -= 3;
            if (currentSquare.y < 0)
                currentSquare.y = 0;
            currentSquare.width += 6;
            currentSquare.height += 6;*/
            
            *character = _original(currentSquare);
            _characterIdx++;
            charFound = true;
        }
        else
            _characterIdx++;
    }
    return character;
}

