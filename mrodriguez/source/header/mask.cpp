#ifndef MASK_CPP_
#define MASK_CPP_
#include "utility.hpp"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace utility;

cv::Mat mask::kirsch(const int size){

        cv::Mat dst;
        std::vector<cv::Mat> VecKirsch;

        switch (size){

                case 3:
                    VecKirsch.push_back(  (Mat_<int>(3,3) << -3,-3, 5, -3, 0, 5, -3, -3, 5) ); //East
                    VecKirsch.push_back(  (Mat_<int>(3,3) << -3, 5, 5, -3, 0, 5, -3, -3, -3) ); //North East
                    VecKirsch.push_back(  (Mat_<int>(3,3) <<  5, 5, 5, -3, 0, -3, -3, -3, -3) ); //North
                    VecKirsch.push_back(  (Mat_<int>(3,3) <<  5, 5, -3, 5, 0, -3, -3, -3, -3) ); //North West
                    VecKirsch.push_back(  (Mat_<int>(3,3) <<  5, -3, -3, 5, 0, -3, 5, -3, -3) ); //West
                    VecKirsch.push_back(  (Mat_<int>(3,3) <<  -3, -3, -3, 5, 0, -3, 5, 5, -3) ); //South West
                    VecKirsch.push_back(  (Mat_<int>(3,3) <<  -3, -3, -3, -3, 0, -3, 5, 5, 5) ); //South
                    VecKirsch.push_back(  (Mat_<int>(3,3) <<  -3, -3, -3, -3, 0, 5, -3, 5, 5) ); //South East
                    break;
                case 5:
                    break;
                case 7:
                    break;
                case 9:
                    break;
                case 11:
                    break;
                 default:
                    break;
        }

        cv::merge(VecKirsch, dst);
        return (dst);
}


#endif