//
//  LPFranceSegmenter.h
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 23/05/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#ifndef __License_Plate_Recognizer__LPFranceSegmenter__
#define __License_Plate_Recognizer__LPFranceSegmenter__

#include <iostream>
#include "LPStandarSegmenter.h"

class LPFranceSegmenter : public LPStandarSegmenter {
    
   
protected:
    // Possible own implementation if the process it's really different from the base class
    
public:
    
    LPFranceSegmenter(Mat inputV) : LPStandarSegmenter(inputV){
        
    }
    
  /**  void run(){
        oImpl.preprocess(&input);
        oImpl.filter(&input);
        
        //** ETC ETC **/
    //}
    
};

#endif /* defined(__License_Plate_Recognizer__LPFranceSegmenter__) */
