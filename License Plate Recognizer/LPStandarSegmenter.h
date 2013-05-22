//
//  LPStandarSegmenterAbstraction.h
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 22/05/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#ifndef __License_Plate_Recognizer__LPStandarSegmenter__
#define __License_Plate_Recognizer__LPStandarSegmenter__

#include <iostream>
#include "LPStandarSegmenterImpl.h"
#include "SegmenterType.h"

#include <opencv2/opencv.hpp>

using namespace cv;

class LPStandarSegmenter {
    LPStandarSegmenterImpl oImpl;
    Mat input;
    
public:
    
    LPStandarSegmenter(Mat inputV){
        oImpl = LPStandarSegmenterImpl();
        input = inputV;
    }
    
    inline void run(){
        oImpl.run(input);
    }
    
    inline vector<Mat> getResult(){
        return oImpl.getResult();
    }
    
};

#endif /* defined(__License_Plate_Recognizer__LPStandarSegmenter__) */
