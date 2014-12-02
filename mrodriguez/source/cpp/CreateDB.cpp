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

    int x1, y1,x2, y2, r,dx,dy;
    x1 = std::atoi(argv[2]);
    y1 = std::atoi(argv[3]);
    x2 = std::atoi(argv[4]);
    y2 = std::atoi(argv[5]);
    dx = std::atoi(argv[6]);
    dy = std::atoi(argv[7]);

    namedWindow("Circulo",1);

    VideoWriter outputVideo;
    outputVideo.open(argv[1], 1196444237 , 80, Size(180,180), true);

    for (int i = 0; i < 80; ++i)
    {
        cout << "Frame: " << i << endl;
        img = Mat::zeros(180,180,CV_8UC3);

        rectangle(img, Point(x1 - (i*dx),y1- (i*dy)),Point(x2- (i*dx),y2- (i*dy)), Scalar( 255, 255, 255 ),-1);
        imshow("Circulo", img);
        waitKey(0);
        outputVideo << img;
    }

    return 0;
}