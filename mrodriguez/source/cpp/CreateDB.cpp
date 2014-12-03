#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../header/utility.hpp"

using namespace cv;
using namespace std;
using namespace utility;


int main(int argc, char const *argv[])
{

    if(argc < 8)
    {
        cout << "./programa <video_out.avi> <x1> <y1> <x2> <y2> <velocidadx> <velocidady>" << endl;
        return(-1);
    }

    Mat img;
    Mat Cuadrado1;
    Mat Cuadrado2;

    int x1, y1,x2, y2,dx,dy;
    x1 = std::atoi(argv[2]);
    y1 = std::atoi(argv[3]);
    x2 = std::atoi(argv[4]);
    y2 = std::atoi(argv[5]);
    dx = std::atoi(argv[6]);
    dy = std::atoi(argv[7]);

    namedWindow("Circulo",1);

    VideoWriter outputVideo;
    outputVideo.open(argv[1], 1196444237 , 80, Size(180,180), true);


    Cuadrado1 = Mat::zeros(x2-x1,y2-y1,CV_8UC3);
    Cuadrado2 = Mat::zeros(y2-y1,x2-x1,CV_8UC3);
    rectangle(Cuadrado1, Point(0,0),Point(x2- x1,y2-y1), Scalar( 255, 0, 0 ),-1);
    rectangle(Cuadrado2, Point(0,0),Point(y2-y1,x2- x1), Scalar( 0, 0, 255 ),-1);
    cv::Mat noise(Cuadrado1.size(), Cuadrado1.type());
    cv::Mat noise2(Cuadrado2.size(), Cuadrado2.type());
    cv::randn(noise, 3, 1000); //mean and variance
    cv::randn(noise2, 3, 1000); //mean and variance
    Cuadrado1 += noise;
    Cuadrado2 += noise;

    for (int i = 0; i < 80; ++i)
    {
        cout << "Frame: " << i << endl;
        img = Mat::zeros(180,180,CV_8UC3);

        for (int ii = 0; ii < Cuadrado1.rows; ++ii)
        {
            for (int jj = 0; jj < Cuadrado1.cols; ++jj)
            {
                //cout << img.row(x1+(dx*i+ii)).col(y1+ (dy*i)+jj)  << endl;
                //cout << Cuadrado1.row(ii).col(jj) << endl;
                Cuadrado1.row(ii).col(jj).copyTo(img.row((x1+(dx*i)+ii)).col(y1+ (dy*i)+jj));
                Cuadrado2.row(ii).col(jj).copyTo(img.row(img.rows - (y1+ (dy*i)+jj)).col(img.cols - (x1+(dx*i)+ii)));
            }
        }

        imshow("Circulo", img);
        waitKey(0);
        outputVideo << img;
    }

    return 0;
}