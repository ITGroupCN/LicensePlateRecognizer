//
//  LPSwitzerlandSegmenter.h
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 23/05/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#ifndef __License_Plate_Recognizer__LPSwitzerlandSegmenter__
#define __License_Plate_Recognizer__LPSwitzerlandSegmenter__

#include <iostream>
#include "LPStandarSegmenter.h"

class LPSwitzerlandSegmenter : public LPStandarSegmenter {
    
    
public:
    
    LPSwitzerlandSegmenter(Mat inputV) : LPStandarSegmenter(inputV){
        
    }
    
};


#endif /* defined(__License_Plate_Recognizer__LPSwitzerlandSegmenter__) */
