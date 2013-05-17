//
//  OCREngine.h
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 03/04/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#ifndef __License_Plate_Recognizer__OCREngine__
#define __License_Plate_Recognizer__OCREngine__

#include <iostream>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


using namespace tesseract;

class OCREngine {
    TessBaseAPI* _engine;
    bool _initialized;
    bool _imagePresent;
    char* languageDataPath;
    char* language;
    
    
public:
    OCREngine(const char* languageDataPath, const char* language);
    void setSingleCharacterMode();
    void feedImage(const unsigned char *imagedata, int bytes_per_pixel, int bytes_per_line, int left, int top, int width, int height);
    char * getText();
    bool isInitialized();
    void clean(void);
    void setWhiteList(const char* whiteList);

       
};

#endif /* defined(__License_Plate_Recognizer__OCREngine__) */
