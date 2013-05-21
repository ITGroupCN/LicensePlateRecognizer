#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <stdlib.h>
#include <stdio.h>
#include "gabor.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "Rectangle.h"

using namespace cv;
using namespace std;
using namespace tesseract;

string BASEPATH = "/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/";

RNG rng(12345);

string genFullPath(char file[]);
void showImageGUI(const String sWindowName, int iStopKey, const Mat oInputImage);
bool DescendingCompare (std::vector<cv::Point> i,std::vector<cv::Point> j) { return (i.size()>j.size()); }

bool squareCompare (Rectangle i, Rectangle j){
    return (i.getCenter().x < j.getCenter().x);
}
void showCroppedContour(Mat image, vector<vector<Point> >contours);

int main( int argc, char** argv )
{

    tesseract::TessBaseAPI *myOCR =
    new tesseract::TessBaseAPI();
    
    
    // [2]
    printf("Tesseract-ocr version: %s\n", myOCR->Version());
    
    // [3]
    if (myOCR->Init(NULL, "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    
    myOCR->SetPageSegMode(tesseract::PageSegMode(10));
    
    Mat image = imread( genFullPath("sample2.jpg").c_str(), 1 );
    Mat src,src_f;
    
    showImageGUI("source", 27, image);
    
    cv::cvtColor(image, src, CV_BGR2GRAY);
    src.convertTo(src_f, CV_32F, 1.0/255, 0);

    
    Mat kernel = mkKernel(21, 1, 95, 0.69, 21);
    
    Mat res = processGabor(src_f,kernel,21);

   // threshold(res, res, 5, 255, CV_THRESH_BINARY);
    
    //dilate(res, res, NULL);

    vector<Vec4i> hierarchy;

    vector<vector<Point> > contours;

    res.convertTo(res, CV_8UC1);
    
    findContours( res, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    Mat drawing = Mat::zeros( res.size(), CV_8UC3 );
    
    
    cout << "# Contours: " << contours.size() << endl;
    
    vector<Rectangle> filteredSquares;
    
    double minWidth = 5000000;

    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, CV_AA, hierarchy, 0, Point() );
        
       
        
   //     line(image, Point(xCentroid,yCentroid), Point(xCentroid+2, yCentroid+2),Scalar(0,0,255));
        
        
        Rect square = boundingRect(contours[i]);
        rectangle(image, square, Scalar(0,0,255));

        if (square.height > (image.rows/3)){
            if (minWidth > square.width){
                minWidth = square.width;
            }
          
            
            Moments mnt = moments(contours[i]);
            
            double xCentroid = mnt.m10/mnt.m00;
            double yCentroid = mnt.m01/mnt.m00;
            
            Point center; center.x = xCentroid; center.y = yCentroid;
            
            Rectangle rectangle = Rectangle(square, center);
            filteredSquares.push_back(rectangle);
 
        }
    }
    
    sort(filteredSquares.begin(), filteredSquares.end(), squareCompare);
    
    cout << "#Candidate squares: " << filteredSquares.size() << endl;
    
    cout << "Min width: " << minWidth << endl;
    threshold(src, src, 50, 255, CV_THRESH_BINARY);

    
    stringstream ocrText;
    for (int i = 0 ; i < filteredSquares.size(); ++i){
        if (filteredSquares[i].getRect().width >= minWidth && filteredSquares[i].getRect().width < minWidth*8){
          //  fillPoly(mask, filteredSquares[i], Scalar(255,255,255));

            
        rectangle(image, filteredSquares[i].getRect(), Scalar(0,0,255));
            Mat letter = src(filteredSquares[i].getRect());
        
          
            

            stringstream str;
            str << BASEPATH << i << ".jpg";
            imwrite(str.str().c_str(), letter);
            
            myOCR->TesseractRect(letter.data, 1, letter.step1(), 0, 0, letter.cols
                                 , letter.rows);
            
            const char *txt = myOCR->GetUTF8Text();
            char *boxes = myOCR->GetBoxText(0);
            
            
            ocrText.put(txt[0]);
            
            
        }
    }
    cout << "Recognized OCR: " << ocrText.str() << endl;
    
    showImageGUI("rects", 27, image);

    
    std::sort(contours.begin(), contours.end(),DescendingCompare);
    
    cv::Mat draw = res;
    
    std::vector<std::vector<cv::Point> > contours_poly(1);
    
    // Determine an approximate polygon for v[0] which is the largest contour
    cv::approxPolyDP( cv::Mat(contours[0]), contours_poly[0], 3, true );
    
    // Draw polygonal contour
    cv::Scalar color = cv::Scalar( 255,0,0 );
    cv::cvtColor(draw, draw, CV_GRAY2RGB);

    cv::drawContours( draw, contours_poly, 0, color, 2, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
    
    for (int i = 0 ; i < contours_poly.size(); ++i){
        for (int j = 0; j < contours_poly[i].size(); ++j){
            cout << "X: " << contours_poly[i][j].x << "  Y: " << contours_poly[i][j].y << endl;
            if (j < 2 || j >= contours_poly[i].size() -2 ){
                line(draw, contours_poly[i][j], Point(contours_poly[i][j].x+3,contours_poly[i   ][j].y+3),Scalar(0,0,255));
            }
        }
    }
    
    showCroppedContour(image, contours);
   
    
   showImageGUI("contours", 27, drawing);
   showImageGUI("res", 27, res);
    
    return 0;
}

void showCroppedContour(Mat image, vector<vector<Point> > contours){
    
    for (int i = 0 ; i < contours.size(); ++i){
        Mat mask = Mat::zeros(image.size(), CV_8UC3 );

    std::vector<std::vector<cv::Point> > contours_poly;
        contours_poly.push_back(contours[i]);
    
    
    
    fillPoly(mask, contours_poly, Scalar(255,255,255));
    Mat masked_image;
    bitwise_and(image, mask, masked_image);
    
    showImageGUI("license", 27, masked_image);
    }

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

string genFullPath(char file[]){
    stringstream fullpath;
    
    fullpath << BASEPATH << file;
    
    return fullpath.str();
}
