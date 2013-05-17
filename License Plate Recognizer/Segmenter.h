//
//  Segmenter.h
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 06/04/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#ifndef __License_Plate_Recognizer__Segmenter__
#define __License_Plate_Recognizer__Segmenter__

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Rectangle.h"
#include "FilterType.h"
#include <limits>

using namespace cv;



class Segmenter {
    Mat _input;
    Mat _original;
    bool _filterApplied;
    bool _contoursPresent;
    vector<vector<Point> > _contours;
    vector<Vec4i> _contourHierarchy;
    vector<Rectangle> _segmentedSquares;

    // Used for letter segmentation
    double minWidth;

    bool _debug;
    
    // Auxiliar functions
    void showWindow(const char* windowName, Mat image);

    
    // Iterator
    bool _isDone;
    int _characterIdx;

    
public:
    Segmenter(const Mat* v);
    void applyFilter(filterType f);
    void preprocess(double thresholdValue);
    void computeContours();
    void showContours();
    void segmentFromContours();
    void enableDebug();
    void disableDebug();
    void showCurrentImage(const char* windowName);
    void viewCandidateSquares(Mat *dest);
    Mat* getNextCharacter();

};



#endif /* defined(__License_Plate_Recognizer__Segmenter__) */
