//
//  OCREngine.cpp
//  License Plate Recognizer
//
//  Created by Mario Orozco Borrego on 03/04/13.
//  Copyright (c) 2013 Mario Orozco Borrego. All rights reserved.
//

#include "OCREngine.h"


OCREngine::OCREngine(const char* languageDataPath, const char* language){

    if (!_initialized){
        _engine = new TessBaseAPI();
        
        if (!_engine->Init(languageDataPath, language)){
            _initialized = true;
            setSingleCharacterMode();
        }
    }
}

void OCREngine::setSingleCharacterMode(){
    if (_initialized)
        _engine->SetPageSegMode(tesseract::PageSegMode(10));
    
    
    
}

void OCREngine::feedImage(const unsigned char *imagedata, int bytes_per_pixel, int bytes_per_line, int left, int top, int width, int height){
    if (_initialized){
        _engine->SetImage(imagedata, width, height, bytes_per_pixel, bytes_per_line);
        _engine->Recognize(0);
        _imagePresent = true;
    }
}

char* OCREngine::getText(){
    if (_initialized && _imagePresent)
        return _engine->GetUTF8Text();
    else
        return NULL;
}

bool OCREngine::isInitialized(){
    return _initialized;
}


void OCREngine::clean(void){
    if (_initialized)
        _engine->Clear();
}

void OCREngine::setWhiteList(const char* whiteList){
    if (_initialized)
        _engine->SetVariable("tessedit_char_whitelist", whiteList);

}

