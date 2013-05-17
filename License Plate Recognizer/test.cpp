#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

cv::Mat mkKernel(int ks, double sig, double th, double lm, double ps)
{
    int hks = (ks-1)/2;
    double theta = th*CV_PI/180;
    double psi = ps*CV_PI/180;
    double del = 2.0/(ks-1);
    double lmbd = lm;
    double sigma = sig/ks;
    double x_theta;
    double y_theta;
    cv::Mat kernel(ks,ks, CV_32F);
    for (int y=-hks; y<=hks; y++)
    {
        for (int x=-hks; x<=hks; x++)
        {
            x_theta = x*del*cos(theta)+y*del*sin(theta);
            y_theta = -x*del*sin(theta)+y*del*cos(theta);
            kernel.at<float>(hks+y,hks+x) = (float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma,2))* cos((2*CV_PI*x_theta)/(lmbd + psi));
        }
    }
    return kernel;
}

int kernel_size=21;
int pos_sigma= 5;
int pos_lm = 50;
int pos_th = 0;
int pos_psi = 90;
cv::Mat src_f;
cv::Mat dest;
RNG rng(12345);

void Process(int , void *)
{
    double sig = pos_sigma;
    double lm = 0.5+pos_lm/100.0;
    double th = pos_th;
    double ps = pos_psi;
    cv::Mat kernel = mkKernel(kernel_size, sig, th, lm, ps);
    cv::filter2D(src_f, dest, CV_32F, kernel);
    cv::imshow("Process window", dest);
    cv::Mat Lkernel(kernel_size*20, kernel_size*20, CV_32F);
    cv::resize(kernel, Lkernel, Lkernel.size());
    Lkernel /= 2.;
    Lkernel += 0.5;
    cv::imshow("Kernel", Lkernel);
    cv::Mat mag;
    cv::pow(dest, 2.0, mag);
    cv::imshow("Mag", mag);
    
    vector<Vec4i> hierarchy;
    
    vector<vector<Point> > contours;

    
    threshold(dest, dest, 5, 255, CV_THRESH_BINARY);
    
    dilate(dest, dest, NULL);
    dest.convertTo(dest, CV_8UC1);
    
    findContours( dest, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    Mat drawing = Mat::zeros( dest.size(), CV_8UC3 );
    
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }
    cv::imshow("contours", drawing);
    
    cout << "Sigma: " << sig << endl;
    cout << "Lambda: " << lm << endl;
    cout << "Theta: " << th << endl;
    cout << "Psi: " << ps << endl;
    cout << "Kernel Size: " << kernel_size << endl;
}

int main(int argc, char** argv)
{
    cv::Mat image = cv::imread("/Users/psylock/Documents/XcodeWorkspace/License Plate Recognizer/sample.jpg",1);
    cv::imshow("Src", image);
    cv::Mat src;
    cv::cvtColor(image, src, CV_BGR2GRAY);
    src.convertTo(src_f, CV_32F, 1.0/255, 0);
    if (!kernel_size%2)
    {
        kernel_size+=1;
    }
    cv::namedWindow("Process window", 1);
    cv::createTrackbar("Sigma", "Process window", &pos_sigma, kernel_size, Process);
    cv::createTrackbar("Lambda", "Process window", &pos_lm, 100, Process);
    cv::createTrackbar("Theta", "Process window", &pos_th, 180, Process);
    cv::createTrackbar("Psi", "Process window", &pos_psi, 360, Process);
    Process(0,0);
    cv::waitKey(0);
    return 0;
}
