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

void Pintar_imgXT(cv::Mat &frame, std::vector<cv::Mat> &vectImg, const int indexImg,const int r, const int c, const int ActualFrame, const int size, const Tcolor &color){
    std::vector<Mat> FrameChannels, dstChannels;
    cv::split(frame, FrameChannels);
    cv::split(vectImg[indexImg], dstChannels);

    dstChannels[0].col(ActualFrame) += FrameChannels[0].col(c);
    dstChannels[1].col(ActualFrame) += FrameChannels[1].col(c);
    dstChannels[2].col(ActualFrame) += FrameChannels[2].col(c);

    int rMax = r + (size-1)/2 ;
     for(int i = r - (size-1)/2; i < rMax; i++)
     {
        if(i < 0 || i >= frame.rows) continue;

        dstChannels[0].col(ActualFrame).at<uchar>(i,0) = std::get<0>(color);
        dstChannels[1].col(ActualFrame).at<uchar>(i,0) = std::get<1>(color);
        dstChannels[2].col(ActualFrame).at<uchar>(i,0) = std::get<2>(color);
     }

    cv::merge(dstChannels, vectImg[indexImg]);
}

void Pintar(Mat &frame, int r, int c, int const size, Tcolor const color){

    vector<Mat> channels;

    split(frame, channels);
    int rMax = r + (size-1)/2 ;
    int cMax = c + (size-1)/2;

    for(int i = r - (size-1)/2; i < rMax; i++){
        for(int j = c - (size-1)/2; j < cMax; j++){

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
    if(argc < 9){
        cout << "Error intente:\n ./programa <Video> <SupportRegionSize> <WindowSearchSize> <MaxSizePixelList> <Paint size> <NameOutFile> <XT Path> <XY Path>"  << endl;
        return (-1);
    }

    VideoCapture VideoFile;
    VideoCapture VideoFileForExtract;

    VideoFile.open(argv[1]);
    VideoFileForExtract.open(argv[1]);
    MaxSizePixelList = std::atoi(argv[4]);
    int PaintSize = std::atoi(argv[5]);
    string NameOutFIle(argv[6]);
    string XTPath(argv[7]);
    string YTPath(argv[8]);


    Mat dstImage, grayImage, dstImageConst;

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


    std::vector<cv::Mat> XT(PixelList.size(), cv::Mat::zeros(Frame.rows, nFrames, Frame.type()));
    std::vector<cv::Mat> YT(PixelList.size(), cv::Mat::zeros(Frame.cols, nFrames, Frame.type()));


    int nFrame = -1, r, c;
    do{
        std::vector<cv::Mat> dstImageSplit(3);


        cvtColor(Frame, grayImage, CV_BGR2GRAY);
        cvtColor(grayImage, dstImage, CV_GRAY2BGR);
        cvtColor(grayImage, dstImageConst, CV_GRAY2BGR);

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

            cout << "Frame: " << nFrame << " | ColorCount: " << colorCount << endl;
            Pintar_imgXT(dstImageConst, XT, colorCount, r, c, nFrame+1, PaintSize, TcolorList[colorCount]);

            Pintar(dstImage, r, c, PaintSize, TcolorList[colorCount]);

            colorCount++;
        }

        outputVideo << dstImage;

        nFrame++;
    }while(VideoFile.read(Frame));


    for (size_t i = 0; i < PixelList.size(); ++i)
    {
        pixel ActualPixel  = PixelList[i];
        string NameXT = XTPath + std::to_string(ActualPixel.first) + "_" + std::to_string(ActualPixel.second) + ".jpg";
        string NameYT = YTPath + std::to_string(ActualPixel.first) + "_" + std::to_string(ActualPixel.second) + ".jpg";

        imwrite( NameXT.c_str(), XT[i] );
        imwrite( NameYT.c_str(), YT[i] );
    }

    return 0;
}