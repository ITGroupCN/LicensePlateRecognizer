//
//  LPStandarSegmenterImpl.h
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 22/05/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#ifndef __License_Plate_Recognizer__LPStandarSegmenterImpl__
#define __License_Plate_Recognizer__LPStandarSegmenterImpl__

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

class LPStandarSegmenterImpl{
    
    vector<Mat> result;
    void initializeThresholds();

    // THRESHOLDS
    int THRESHOLD_BINARIZE;
    double SQUARE_RATIO;
    double MIN_SQUARE_HEIGHT_PERCENTAGE;
    double MIN_SQUARE_WIDTH_PERCENTAGE;
    int MAX_ALLOWED_HEIGHT_VARIATION;
    
public:
    
    LPStandarSegmenterImpl(){
        
    }
    void run(Mat input);
    void setResult(vector<Mat> v);
    inline vector<Mat> getResult(){
        return result;
    }
    
};

#endif /* defined(__License_Plate_Recognizer__LPStandarSegmenterImpl__) */
