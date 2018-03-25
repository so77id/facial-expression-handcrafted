#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

typedef tuple<int,int,int> Tcolor;

double CalcError(const string& ImgName, const string& Pre1Name, const string& Pre2Name, const string& Pre3Name, const Tcolor& ActualColor, const string& ImgOut)
{

    Mat ImgOriginal;
    Mat ImgPre1, ImgPre2, ImgPre3;

    ImgOriginal = imread(ImgName, CV_LOAD_IMAGE_COLOR);
    ImgPre1 = imread(Pre1Name, CV_LOAD_IMAGE_COLOR);
    ImgPre2 = imread(Pre2Name, CV_LOAD_IMAGE_COLOR);
    ImgPre3 = imread(Pre3Name, CV_LOAD_IMAGE_COLOR);

    Mat ImgOutMat = Mat::zeros(ImgOriginal.size(), ImgOriginal.type());
    Mat LastImgOut;

    vector<Mat> VecOriginal, VecPre1, VecPre2, VecPre3, VecOut;
    cv::split(ImgOriginal,VecOriginal);
    cv::split(ImgPre1,VecPre1);
    cv::split(ImgPre2,VecPre2);
    cv::split(ImgPre3,VecPre3);
    cv::split(ImgOutMat, VecOut);

    vector <int> OriValues, PreValues1, PreValues2, PreValues3, PromValues;

    int SumOri, SumPre1, SumPre2, SumPre3;
    int ConOri, ConPre1, ConPre2, ConPre3;

    for (int i = 0; i < ImgOriginal.cols; ++i)
    {
            SumOri = SumPre1 = SumPre2 = SumPre3 = 0;
            ConOri  = ConPre1  = ConPre2  = ConPre3 = 0;

            VecOriginal[0].col(i).copyTo(VecOut[0].col(i));
            VecOriginal[1].col(i).copyTo(VecOut[1].col(i));
            VecOriginal[2].col(i).copyTo(VecOut[2].col(i));


            for (int j = 0; j < ImgOriginal.col(0).rows; ++j)
            {

                    if(  ((int)VecOriginal[0].at<uchar>(j,i) == std::get<0>(ActualColor) ) &&
                         ((int)VecOriginal[1].at<uchar>(j,i) == std::get<1>(ActualColor) ) &&
                         ((int)VecOriginal[2].at<uchar>(j,i) == std::get<2>(ActualColor) ))
                    {
                        //cout << "Encontre " << i << " " << j << endl;
                        SumOri +=j;
                        ConOri++;
                    }

                     if(  ((int)VecPre1[0].at<uchar>(j,i) == std::get<0>(ActualColor) ) &&
                          ((int)VecPre1[1].at<uchar>(j,i) == std::get<1>(ActualColor) ) &&
                          ((int)VecPre1[2].at<uchar>(j,i) == std::get<2>(ActualColor) ))
                      {
                            //cout << "Encontre " << i << " " << j << endl;
                            SumPre1 +=j;
                            ConPre1++;
                      }

                     if(  ((int)VecPre2[0].at<uchar>(j,i) == std::get<0>(ActualColor) ) &&
                          ((int)VecPre2[1].at<uchar>(j,i) == std::get<1>(ActualColor) ) &&
                          ((int)VecPre2[2].at<uchar>(j,i) == std::get<2>(ActualColor) ))
                      {
                            //cout << "Encontre " << i << " " << j << endl;
                            SumPre2 +=j;
                            ConPre2++;
                      }

                     if(  ((int)VecPre3[0].at<uchar>(j,i) == std::get<0>(ActualColor) ) &&
                          ((int)VecPre3[1].at<uchar>(j,i) == std::get<1>(ActualColor) ) &&
                          ((int)VecPre3[2].at<uchar>(j,i) == std::get<2>(ActualColor) ))
                      {
                            //cout << "Encontre " << i << " " << j << endl;
                            SumPre3 +=j;
                            ConPre3++;
                      }
            }

            OriValues.push_back(SumOri / ConOri);
            PreValues1.push_back(SumPre1 / ConPre1);
            PreValues2.push_back(SumPre2 / ConPre2);
            PreValues3.push_back(SumPre3 / ConPre3);

            int pos = ((SumPre1 / ConPre1) + (SumPre2 / ConPre2) + (SumPre3 / ConPre3)) / 3.0;

            VecOut[0].at<uchar>(pos,i) = 255 - std::get<0>(ActualColor);
            VecOut[1].at<uchar>(pos,i) = 255 - std::get<1>(ActualColor);
            VecOut[2].at<uchar>(pos,i) = 255 - std::get<2>(ActualColor);
            //cout << "Guardando en: " << i << " " << pos  << " | " << int(VecOut[0].at<uchar>(pos,i))<< endl;
    }

    double EstandarDesviation = 0.0;

    for (size_t i = 0; i < OriValues.size(); ++i)
    {
        EstandarDesviation += ((((PreValues1[i] + PreValues2[i] + PreValues3[i])*1.0) / 3.0) -  (OriValues[i] * 1.0)) * ((((PreValues1[i] + PreValues2[i] + PreValues3[i])*1.0) / 3.0) - (OriValues[i] * 1.0));
    }

    cv::merge(VecOut, LastImgOut);
    imwrite(ImgOut,LastImgOut);

    return( (EstandarDesviation / (OriValues.size()*1.0)) );
}



int main(int argc, char const *argv[])
{

    map<string,Tcolor> TcolorList;
    TcolorList["rojo"] = std::make_tuple(0,0,255);
    TcolorList["azul"] = std::make_tuple(255,0,0);
    TcolorList["verde"] = std::make_tuple(0,255,0);
    TcolorList["amarillo"] = std::make_tuple(0,215,255);
    TcolorList["dorado"] = std::make_tuple(0,165,255);
    TcolorList["rosado"] = std::make_tuple(238,130,238);


    if(argc < 5)
    {
        cout << "./programa <Path Rayos Originales> <Path Rayos pre 1> <Path Rayos pre  2> <Path Rayos pre 3> <path rayos comparados>" << endl;
        return(-1);
    }


    string PathOriginal(argv[1]);
    string PathPre1(argv[2]);
    string PathPre2(argv[3]);
    string PathPre3(argv[4]);
    string PathOut(argv[5]);

//    cout << PathOriginal << endl;
//    cout << PathPre1 << endl;
//    cout << PathPre2 << endl;
//    cout << PathPre3  << endl;

    string PathWithRsWs;
    vector<int> XT, YT;
    vector<string> Color;

    int xt, yt, rs, ws;
    size_t coorSize, rswsSize;
    string color;

    cin >> coorSize;

    for(size_t i = 0; i < coorSize; i++)
    {
        cin >> xt >> yt >> color;
       // cout << xt << " " << yt << " " << color << endl;
        XT.push_back(xt);
        YT.push_back(yt);
        Color.push_back(color);
    }

    cin >> rswsSize;


    cout << "rs ws errorXT errorYT errorG" << endl;

    for(size_t j = 0; j < rswsSize; j++)
    {
        cin >> rs >> ws;
        PathWithRsWs = PathOriginal + "rs" + std::to_string(rs) + "ws" + std::to_string(ws) + "/";
        string PathOutRsWs = PathOut +  "rs" + std::to_string(rs) + "ws" + std::to_string(ws) + "/";
        double XTError, YTError;

        XTError = YTError = 0.0;
        for(size_t i = 0; i < XT.size(); i++)
        {
    //      cout << XT << "->" << YT << "->" << Color << endl;
            {//BLoque XT
                    string ImgName( PathWithRsWs + "XT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string Pre1Name( PathPre1 + "XT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string Pre2Name( PathPre2 + "XT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string Pre3Name( PathPre3 + "XT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string ImgOut( PathOutRsWs + "XT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
           //         cout << ImgName << endl;
           //         cout << Pre1Name << endl;
           //         cout << Pre2Name << endl;
           //         cout << Pre3Name << endl;

                    Tcolor ActualColor = TcolorList[Color[i]];

    //                cout << std::get<0>(ActualColor) << " " << std::get<1>(ActualColor) << " " << std::get<2>(ActualColor) << endl;

                    double  Error = CalcError(ImgName, Pre1Name, Pre2Name, Pre3Name, ActualColor, ImgOut);
                    //cout << Error << endl;

                    XTError += Error;
            }

            {//BLoque XT
                    string ImgName( PathWithRsWs + "YT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string Pre1Name( PathPre1 + "YT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string Pre2Name( PathPre2 + "YT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string Pre3Name( PathPre3 + "YT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );
                    string ImgOut( PathOutRsWs + "YT/" + std::to_string(XT[i]) + "_" + std::to_string(YT[i]) + ".png" );

           //         cout << ImgName << endl;
           //         cout << Pre1Name << endl;
           //         cout << Pre2Name << endl;
           //         cout << Pre3Name << endl;

                    Tcolor ActualColor = TcolorList[Color[i]];

    //                cout << std::get<0>(ActualColor) << " " << std::get<1>(ActualColor) << " " << std::get<2>(ActualColor) << endl;

                    double  Error = CalcError(ImgName, Pre1Name, Pre2Name, Pre3Name, ActualColor, ImgOut);
                    //cout << Error << endl;

                    YTError += Error;
            }

        }

        cout << rs << " ";
        cout << ws << " ";
        cout << (XTError/(XT.size()*1.0)) << " ";
        cout << (YTError/(YT.size()*1.0)) << " ";

        cout << ((YTError + XTError)/((XT.size()*1.0) + (YT.size()*1.0))) << endl;
    }

    return 0;
}