#include <cmath>
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
    if(argc < 3)
    {
        cout << "./programa <video> <img salida> " << endl;
        return(-1);
    }

    string videoName(argv[1]);
    string imgOut(argv[2]);
    Mat frame;


    VideoCapture VideoFile;

    VideoFile.open(videoName);

    if(!VideoFile.isOpened())
    {
        cout << "error con el video de entrada" << endl;
        return(-1);
    }

    VideoFile.read(frame);

    namedWindow("vista",1);
    imshow("vista",frame);
    waitKey(0);
    imwrite(imgOut,frame);

    return 0;
}