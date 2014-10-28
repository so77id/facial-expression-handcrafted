#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


#include "../header/RaysExtractor2.hpp"
#include "../header/utility.hpp"

using namespace cv;
using namespace std;
using namespace utility;



size_t MaxSizePixelList;
vector<pixel> PixelList;
                    //BGR
typedef tuple<int,int,int> Tcolor;
//                                            azul          verde          rojo            gold             naranjo          violeta
vector<Tcolor> TcolorList = { std::make_tuple(255,0,0),
                                              std::make_tuple(0,255,0),
                                              std::make_tuple(0,0,255),
                                              std::make_tuple(0,215,255),
                                              std::make_tuple(0,165,255),
                                              std::make_tuple(238,130,238),
                                              std::make_tuple(147,20,255),
                                              std::make_tuple(128,0,128),
                                              std::make_tuple(255,255,224),
                                              std::make_tuple(0,128,128)
                                          };



void Pintar(Mat &frame, int r, int c, int const size, Tcolor const color){

    vector<Mat> channels;

    split(frame, channels);
    int rMax = r + size;
    int cMax = c + size;

    for(int i = r; i < rMax; i++){
        for(int j = c; j < cMax; j++){

            if(i < 0 || j < 0 || i >= frame.rows || j >= frame.cols) continue;

            channels[0].at<uchar>(i,j) = std::get<0>(color);
            channels[1].at<uchar>(i,j) = std::get<1>(color);
            channels[2].at<uchar>(i,j) = std::get<2>(color);
        }
    }

    merge(channels, frame);

}


void OnClickCallBack(int event, int x, int y, int flags, void* param)
{
    if(flags || param){ };

     if  ( event == EVENT_LBUTTONDOWN )
     {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;

        if(PixelList.size() < MaxSizePixelList)
        {
            cout << "Guarde el ultimo " << endl;
            PixelList.push_back(std::make_pair(y,x));
        }
     }
}


int main(int argc, char const *argv[])
{
    if(argc < 7){
        cout << "Error intente:\n ./programa <Video> <SupportRegionSize> <WindowSearchSize> <MaxSizePixelList> <Paint size> <NameOutFile>"  << endl;
        return (-1);
    }

    VideoCapture VideoFile;
    VideoCapture VideoFileForExtract;

    VideoFile.open(argv[1]);
    VideoFileForExtract.open(argv[1]);
    MaxSizePixelList = std::atoi(argv[4]);
    int PaintSize = std::atoi(argv[5]);
    string NameOutFIle(argv[6]);

    Mat dstImage, grayImage;

    namedWindow("Eleccion",1);
    setMouseCallback("Eleccion", OnClickCallBack, NULL);


    if(!VideoFile.isOpened() || !VideoFileForExtract.isOpened()){
        cout << "No abre el video" << endl;
        return (-1);
    }

    Mat Frame, DstFrame;
    int nFrames = VideoFile.get(CV_CAP_PROP_FRAME_COUNT);


    //vector<Mat> spl;

    VideoFile.read(Frame);

    int ex = static_cast<int>(VideoFile.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
    Size S = Size((int) VideoFile.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) VideoFile.get(CV_CAP_PROP_FRAME_HEIGHT));
    VideoWriter outputVideo;
    outputVideo.open(NameOutFIle, ex, VideoFile.get(CV_CAP_PROP_FPS), S, true);

     if (!outputVideo.isOpened())
    {
        cout  << "NO se puede abrir un video para escribir" << endl;
        return -1;
    }

    imshow("Eleccion", Frame);


    while(waitKey(0) != 13);

    cout << "Comenzando la extraccion de rayos" << endl;

    RaysExtractor Extractor;
    MapRaysFlux RaysRoi = Extractor.Extract(VideoFileForExtract,std::atoi(argv[2]), std::atoi(argv[3]), true);


    vector<Mat> XT(PixelList.size(), Mat::zeros(Frame.rows, nFrames, Frame.type()));
    vector<Mat> YT(PixelList.size(), Mat::zeros(Frame.cols, nFrames, Frame.type()));


    int nFrame = -1, r, c;
    do{
        vector<Mat> dstImageSplit(3);


        cvtColor(Frame, grayImage, CV_BGR2GRAY);
        cvtColor(grayImage, dstImage, CV_GRAY2BGR);

        int colorCount = 0;
        for (std::vector<pixel>::iterator i = PixelList.begin(); i != PixelList.end(); ++i)
        {
            pixel actual = *i;

            if(nFrame == -1){
                r = actual.first;
                c = actual.second;
            }
            else{
                r = RaysRoi[actual][nFrame].first;
                c = RaysRoi[actual][nFrame].second;
            }
            Pintar(dstImage, r,c,PaintSize,TcolorList[colorCount]);

            colorCount++;
        }

        outputVideo << dstImage;

        nFrame++;
    }while(VideoFile.read(Frame));

    return 0;
}