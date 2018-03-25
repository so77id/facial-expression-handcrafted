#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../header/utility.hpp"

using namespace cv;
using namespace std;
using namespace utility;


bool TransformVideo(const string& video_in, const string& video_out)
{
        Mat frame;
        Mat lbpFrame;

        VideoCapture VideoIn(video_in);
        VideoWriter VideoOut;

        int ex = static_cast<int>(VideoIn.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
        Size S = Size((int) VideoIn.get(CV_CAP_PROP_FRAME_WIDTH) - 2,    // Acquire input size
                      (int) VideoIn.get(CV_CAP_PROP_FRAME_HEIGHT) - 2 );

        VideoOut.open(video_out, ex, VideoIn.get(CV_CAP_PROP_FPS), S, true);

        while(VideoIn.read(frame))
        {
            std::vector<Mat> FrameChannels, dstChannels;
            cv::split(frame, FrameChannels);

            dstChannels.push_back(lbp::OLBP(FrameChannels[0]));
            dstChannels.push_back(lbp::OLBP(FrameChannels[1]));
            dstChannels.push_back(lbp::OLBP(FrameChannels[2]));

            cv::merge(dstChannels, lbpFrame);

            VideoOut.write(lbpFrame);
        }

        return(true);
}


int main(int argc, char const *argv[])
{
     if(argc < 3){
        cout << "Error intente:\n ./programa <video_list_in> <path_videos_lbp>"  << endl;
        return (-1);
    }


    string video_list(argv[1]);
    string pathVideosLbp(argv[2]);
    string video_in, video_out;

    ifstream VideosIn(video_list);
    ofstream VideosOut(pathVideosLbp + "video_list_lbp.txt");

    if(!VideosIn.good() || !VideosOut.good())
    {
        cout << "No se pudo abrir los videos" << endl;
        return(-1);
    }

    size_t i = 0;
    while(true)
    {
        string path_video, path_new_video;
        size_t id, class_id, nframes;
        VideosIn >> path_video >> id >> class_id >> nframes;

        if(VideosIn.eof()) break;

        path_new_video = pathVideosLbp + "rigid_test_lbp"+ std::to_string(id) +".avi";

        cout << "Transformando el video: " << id << endl;

        if(TransformVideo(path_video, path_new_video))
        {
            VideosOut << (i++ == 0 ? "" : "\n" ) <<  path_new_video << " " << id << " " << class_id << " " << nframes;
        }
    }

    VideosIn.close();
    VideosOut.close();

    return 0;
}