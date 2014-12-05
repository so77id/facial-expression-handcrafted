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
    if(argc < 2)
    {
        cout << "./programa path/videos_clusters" << endl;
        return(-1);
    }

    string path(argv[1]);

    for (int i = 1; i < 7; ++i)
    {
        string video = path + "E" + std::to_string(i) + ".png";
        string out = path + "E" + std::to_string(i) + "_ec.png";
        Mat src,dst;

        src = imread(video,1);
        cvtColor( src, src, CV_BGR2GRAY );

        /// Apply Histogram Equalization
        equalizeHist( src, dst );

        /// Display results
        namedWindow( "src", CV_WINDOW_AUTOSIZE );
        namedWindow( "dst", CV_WINDOW_AUTOSIZE );

        imshow("src", src);
        imshow("dst",dst);

        imwrite(out, dst);
        waitKey(0);

    }

    return(0);
}