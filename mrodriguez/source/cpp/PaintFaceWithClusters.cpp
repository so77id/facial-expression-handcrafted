#include <iostream>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../header/utility.hpp"
#include "../header/kFoldCrossValidation_new.hpp"

using namespace cv;
using namespace std;
using namespace utility;



int main(int argc, char const *argv[])
{
    if(argc < 10)
    {
        cout << "./programa <utl_video> <video id> <rayos> <nClusters> <kFoldPath> <img out> <Extrac clusters?> <Path_vocabulary in or out> <#Clusters a pintar>" << endl;
        return(-1);
    }
    int SRsize = 3;
    string url_video(argv[1]);
    size_t video_id = std::atoi(argv[2]);
    string MicroFileName(argv[3]);
    size_t nClusters = std::atoi(argv[4]);
    string kFoldPath(argv[5]);
    string imgpath(argv[6]);
    int Extract_clusters = std::atoi(argv[7]);
    string VocabularyFileName(argv[8]);
    size_t ClustersPintar = std::atoi(argv[9]);

    Mat Vocabulary;


    kFoldCrossValidation kFCV;

    cout << "Cargando micro: " << MicroFileName << endl;
    kFCV.LoadMicroDescriptors(MicroFileName);
    cout << "Cangando kfold" << kFoldPath << endl;
    kFCV.LoadKfolds(kFoldPath);
    cout << "Moviendo los rayos al map" << endl;
    VideoRayFlux  VideosRays = std::move( kFCV.GetVideosRays());
    cout << "Moviendo los dataset" << endl;
    MapDataSet MDS = std::move(kFCV.GetMDS());
    DataSet DS = MDS[1];

    map<size_t,vector<size_t>> RayoCluster;
    map<size_t, pixel> RayoPixel;
    Macrodescriptor Macro(nClusters,0);

    size_t RaysSize = VideosRays.begin()->second.begin()->size()*2;
    size_t TrainSize = 0;

    cout << "abriendo el video" << endl;

    VideoCapture VideoFile;
    VideoFile.open(url_video);

    Mat frame0;

    VideoFile >> frame0;

    Mat img = cv::Mat::zeros(frame0.rows,frame0.cols, CV_8UC1);

    //Calculando tamaño de las Mat par Train y Test
    for (SetIterator SIter = DS.first.begin(); SIter != DS.first.end(); ++SIter)
    {
        TrainSize += VideosRays[*SIter].size();
    }

    cout << "TrainSize: " << TrainSize << endl;

    size_t i,j,id;
    bool debug_ = true;


    cv::Mat TrainData(TrainSize,RaysSize,CV_32F);
    cv::Mat VideoData(VideosRays[video_id].size(),RaysSize,CV_32F);

    MapMacrodescriptors MMacros;

    if(debug_) cout << "raysize: " << RaysSize << endl;

    //LLenando la Mat de Train
    if(debug_) cout << "Creando matrix de train" << endl;

    if( Extract_clusters == 0)
    {
            i = 0;
            for (SetIterator SIter = DS.first.begin(); SIter != DS.first.end(); ++SIter)
            {
                MMacros[*SIter] = Macrodescriptor(nClusters,0);

                for (ListRaysFlux::iterator LIter = VideosRays[*SIter].begin(); LIter != VideosRays[*SIter].end(); ++LIter)
                {

                    j = 0;
                    for(RayFlux::iterator RIter = LIter->begin(); RIter != LIter->end(); ++ RIter)
                    {
                                    TrainData.at<float>(i,j) = RIter->first; j++;
                                    TrainData.at<float>(i,j) = RIter->second; j++;
                    }

                    i++;
                }
            }
            TermCriteria tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001);
            size_t retries = 1;
            size_t flags = KMEANS_PP_CENTERS;

            BOWKMeansTrainer bowTrainer(nClusters, tc, retries,flags);
            bowTrainer.add(TrainData);

            cout << "Obteniendo clusters" << endl;
            Vocabulary = bowTrainer.cluster();
            FileStorage fs(VocabularyFileName, FileStorage::WRITE);
            cout << "Guardando el voacabulario" << endl;
            fs << "vocabulary" << Vocabulary;
    }
    else
    {
            cout << "Cargando vocabulario " << endl;
            FileStorage fs(VocabularyFileName, FileStorage::READ);
            fs["vocabulary"] >> Vocabulary;
    }
/*

*/
    i = 0;
    for (ListRaysFlux::iterator LIter = VideosRays[video_id].begin(); LIter != VideosRays[video_id].end(); ++LIter)
    {

        j = 0;
        for(RayFlux::iterator RIter = LIter->begin(); RIter != LIter->end(); ++ RIter)
        {
                        VideoData.at<float>(i,j) = RIter->first; j++;
                        VideoData.at<float>(i,j) = RIter->second; j++;
        }

        i++;
    }


    //Match para el train
    if(debug_) cout << "Matcheando" << endl;

        cv::Ptr<cv::DescriptorMatcher> matcher = new cv::BFMatcher(cv::NORM_L2);
        matcher->add(std::vector<cv::Mat>(1, Vocabulary));

        vector<DMatch> matchesTrain;
        matcher->match(VideoData,matchesTrain);



        for (j = 0; j < VideosRays[video_id].size(); ++j)
        {
            id = matchesTrain[j].trainIdx;
            RayoCluster[id].push_back(j);
            Macro[id]++;
        }

        //Imagen completa de valor 255

        int z = 0;
        for(i = (SRsize-1)/2; i < frame0.rows - (SRsize-1)/2; i++ )
        {
            for(j = (SRsize-1)/2; j < frame0.cols -(SRsize-1)/2; j++)
            {
                pixel ActualPixel = make_pair(i,j);
                RayoPixel[z] = ActualPixel;
                //MovimentMap[ActualPixel] = ActualPixel;
                z++;
            }
        }

        cout << "Macro size: " << Macro.size() << endl;
        for(int i = 0; i < Macro.size(); i++)
        {
            cout << Macro[i] << " ";
        }cout << endl;

        int step = 255 / ClustersPintar;

        for(i = 0; i < ClustersPintar && i < Macro.size(); i++)
        {
            int Max = 0;
            int iMax = 0;

            for(j = 0; j < Macro.size(); j++)
            {
                if(Macro[j] >= Max)
                {
                    iMax = j;
                    Max = Macro[j];
                }
            }

            int value = 255 - (i*step) ;
            cout << "Cluster: " << iMax << " Frecuencia: " << Macro[iMax] << " Valor asociado: " << value << endl;
            Macro[iMax] = 0;

            for (std::vector<size_t>::iterator rpiter = RayoCluster[iMax].begin(); rpiter != RayoCluster[iMax] .end(); ++rpiter)
            {
                        pixel Actual  = RayoPixel[*rpiter];
                        img.at<uchar>(Actual.first,Actual.second) = value;
                        //cout << "pixel x: " << Actual.first << " y: " << Actual.second << " Valor: " << i << endl;
            }
        }

        imwrite(imgpath, img);

    return 0;
}