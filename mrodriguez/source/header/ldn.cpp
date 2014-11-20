#ifndef LDN_CPP_
#define LDN_CPP_
#include "utility.hpp"

using namespace cv;
using namespace utility;

template<typename _Tp>
void ldn::LDN_(const cv::Mat& src, cv::Mat& dst, cv::Mat& mask){

    int size = mask.rows, halfSize = (size - 1)/2, imin, imax, bit;
    _Tp max, min;
    cout << src << endl;

//    cv::Mat windows = Mat::zeros(size,size, mask.type());
    dst = Mat::zeros(src.rows-(2*halfSize), src.cols-(2*halfSize), src.type());

    std::vector<cv::Mat> vecMask;
    cv::split(mask,vecMask);

    cv::Mat windows = Mat::zeros(size,size, vecMask[0].type());


    for(int i = halfSize; i < src.rows - halfSize; ++i)
    {
        for(int j = halfSize; j < src.cols - halfSize; ++j)
        {
            min = std::numeric_limits<_Tp>::max();
            max = std::numeric_limits<_Tp>::min();

            for(int wi = 0; wi < size; wi++)
                for(int wj = 0; wj < size; wj++){
                    //cout << (int)src.at<_Tp>(i+wi,j+wj) << endl;
                    windows.at<_Tp>(i, j) = src.at<_Tp>(i+wi, j+wj);
                }

            for (size_t v = 0; v < vecMask.size(); ++v)
            {
                //cout << windows.cols << " " << windows.rows << " " << windows.type() << endl;
                //cout << vecMask[v].cols << " " << vecMask[v].rows << " " << vecMask[v].type() << endl;

                cout << windows << endl;
                cout << vecMask[v] << endl;

                _Tp actual = windows.dot(vecMask[v]);
                cout << int(actual) << endl;

                if(actual < min)
                {
                    min = actual;
                    imin = v;
                }
                else if(actual > max)
                {
                    max = actual;
                    imax = v;
                }
            }

            dst.at<_Tp>(i-halfSize, j-halfSize) = static_cast<_Tp> ( 8*imax + imin );
            cout <<  (int)static_cast<_Tp> ( 8*imax + imin ) << endl;
        }
        cout << "Sali 1" << endl;
    }
    cout << "Sali general" << endl;
    cout << dst << endl;
    namedWindow( "2", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "2", dst );

    cin >> imax;
}



void ldn::LDN(const cv::Mat& src, cv::Mat& dst, cv::Mat& mask)
{
    int size = mask.rows;
    int halfSize = (size-1)/2;
    int imin,imax;

    float max, min;

    std::vector<cv::Mat> vecMask;
    cv::split(mask,vecMask);

    dst = Mat::zeros(src.rows, src.cols, src.type());
    cv::Mat windows = Mat::zeros(size,size, vecMask[0].type());


    dst.at<uchar>(1000,1000);

    for(int i = halfSize; i < src.rows - halfSize; ++i)
    {
        for(int j = halfSize; j < src.cols - halfSize; ++j)
        {
            cout << "1" << endl;
            min = std::numeric_limits<float>::max();
            max = std::numeric_limits<float>::min();

            for(int wi = 0; wi < size; wi++)
                for(int wj = 0; wj < size; wj++){
                    //cout << (int)src.at<_Tp>(i+wi,j+wj) << endl;
                    windows.at<uchar>(i, j) = src.at<uchar>(i+wi, j+wj);
                }

            for (size_t v = 0; v < vecMask.size(); ++v)
            {
                //cout << windows.cols << " " << windows.rows << " " << windows.type() << endl;
                //cout << vecMask[v].cols << " " << vecMask[v].rows << " " << vecMask[v].type() << endl;

                //cout << windows << endl;
                //cout << vecMask[v] << endl;

                float actual = windows.dot(vecMask[v]);
                //cout << int(actual) << endl;

                if(actual < min)
                {
                    min = actual;
                    imin = v;
                }
                else if(actual > max)
                {
                    max = actual;
                    imax = v;
                }
            }
            cout << "Guardando en: " << i - halfSize << "," << j - halfSize << endl;
            cout << "\t rows: " << dst.rows << " cols: " << dst.cols << endl;
            cout << "suma: " << (8*imax + imin) << endl;
            dst.at<uchar>(i-halfSize, j-halfSize) =  ( 8*imax + imin );
        }
    }
}



/*
void ldn::LDN(const cv::Mat& src, cv::Mat& dst, cv::Mat& mask) {
    switch(src.type()) {
        case CV_8SC1: ldn::LDN_<char>(src, dst, mask); break;
        case CV_8UC1: ldn::LDN_<unsigned char>(src, dst, mask); break;
        case CV_16SC1: ldn::LDN_<short>(src, dst, mask); break;
        case CV_16UC1: ldn::LDN_<unsigned short>(src, dst, mask); break;
        case CV_32SC1: ldn::LDN_<int>(src, dst, mask); break;
        case CV_32FC1: ldn::LDN_<float>(src, dst, mask); break;
        case CV_64FC1: ldn::LDN_<double>(src, dst, mask); break;
    }
}*/

cv::Mat ldn::LDN(const cv::Mat& src, cv::Mat& mask) { cv::Mat dst; ldn::LDN(src, dst, mask); return dst; }

#endif