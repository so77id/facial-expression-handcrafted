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
    if(argc < 4)
    {
        cout << "./programa <video> <img salida> <n frame a representar>" << endl;
        return(-1);
    }

    string videoName(argv[1]);
    string imgOut(argv[2]);
    int nFrames = std::atoi(argv[3]);

    VideoCapture VideoFile;

    VideoFile.open(videoName);

    if(!VideoFile.isOpened())
    {
        cout << "error con el video de entrada" << endl;
        return(-1);
    }


    int frames = VideoFile.get(CV_CAP_PROP_FRAME_COUNT);

    int step = std::ceil(frames / nFrames);

    Mat frame,dst;

    VideoFile.read(frame);

    dst = Mat::zeros(  frame.rows,  frame.cols * nFrames, frame.type() );
    int z = 0;
    int con = step;
    do
    {
        if(con == step)
        {
            for (int i = 0; i < frame.rows; ++i)
            {
                for (int j = 0; j < frame.cols; ++j)
                {
                    frame.row(i).col(j).copyTo(dst.row(i).col(j + (z*frame.cols)));
                }
            }
            z++;
            con = 0;
        }
        con++;
   }
    while(VideoFile.read(frame));

    namedWindow("vista",1);
    imshow("vista",dst);
    waitKey(0);
    imwrite(imgOut,dst);



    return 0;
}