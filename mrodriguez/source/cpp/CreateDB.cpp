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

    Mat img;

    int x, y, r;
    x = 150;
    y = 150;
    r = 20;

    namedWindow("Circulo",1);

    VideoWriter outputVideo;
    outputVideo.open("salida.avi", 1196444237 , 80, Size(180,180), true);

    for (int i = 0; i < 80; ++i)
    {
        cout << "Frame: " << i << endl;
        img = Mat::zeros(180,180,CV_32F);

        circle(img, Point( x - i,y), r ,Scalar( 255, 255, 255 ),-1);
        imshow("Circulo", img);
        waitKey(0);
        outputVideo << img;
    }

    return 0;
}