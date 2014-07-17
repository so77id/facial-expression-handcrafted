//hasta aca estamos ok
#include "opencv2/opencv.hpp"
#include <vector>
#include <utility>
#include <limits>
#include <cmath>
#include "utility.hpp"

using namespace cv;
using namespace std;
using namespace utility;


template<typename T>
class RaysExtractor
{
	private:
		T MSE(T, T);
		T SumMSE(const ROI&, const ROI&, const Mat&, const Mat &);
		pair<float,float> ComputeFlow(const pair<int,int> &, const ROI &,const ROI &, const Mat &, Mat &,const int,const int);
	public:
		vector<vector<pair<float,float>>> Extract(VideoCapture &,const int, const int);
		vector<pair<float,float>> Normalize(const vector<pair<float,float>>,const int);
};


template<typename T>
T RaysExtractor<T>::MSE(T intensity0, T intensity1){
	return (pow(intensity0 - intensity1,2));
}


template<typename T>
T RaysExtractor<T>::SumMSE(const ROI& SupportRegion, const ROI& RoiWS,const Mat &ISR,const Mat &IWS){
	T sum = 0;
	for (int iSR = SupportRegion.RowsBegin(), iWS = RoiWS.RowsBegin();
			 iSR < SupportRegion.RowsEnd() && iWS < RoiWS.RowsEnd(); 
			 iSR++ , iWS++){

		for (int jSR = SupportRegion.ColsBegin(), jWS = RoiWS.ColsBegin();
				 jSR < SupportRegion.ColsEnd() && jWS < RoiWS.ColsEnd();
				 jSR++, jWS++)
		{
			sum += MSE(ISR.at<T>(iSR,jSR), IWS.at<T>(iWS,jWS));
		}
	}

	return (sum);
}

//SR SupportRegion
//WS Windows Search
template<typename T>
pair<float,float> RaysExtractor<T>::ComputeFlow(const pair<int,int> &point0, const ROI &SR ,const ROI &WS, const Mat &ISR, Mat &IWS,const int size, const int nFrames){
	
	vector<pair<T,pair<int,int>>> MSE_vector;	
	using MSE_vector_iterator = typename std::vector<pair<T,pair<int,int> > >::iterator;
	

	for(int i = WS.RowsBegin(); i < WS.RowsEnd(); i++){
		for (int j = WS.ColsBegin(); j < WS.ColsEnd(); j++){
			
			ROI roiWS = CreateRoi(IWS,i,j,size);

			T mse = SumMSE(SR,roiWS,ISR,IWS);
			MSE_vector.push_back( std::make_pair(mse, make_pair(i,j) ) );
		}
	}


	T min = std::numeric_limits<T>::max();
	pair<int,int> point1;

	for (MSE_vector_iterator i = MSE_vector.begin(); i != MSE_vector.end(); ++i)
	{
		if(i->first <= min){
			min = i->first;
			point1 = i->second;
		}
	}

	//cout << (point0.first - point1.first) << endl;
	return( std::make_pair( float(point0.first - point1.first) / float(nFrames), float(point0.second - point1.second) / float(nFrames) ) );
}


template<typename T>
vector<vector<pair<float,float>>> RaysExtractor<T>::Extract(VideoCapture &video,const int size, const int SizeNorm){
	
	if(!video.isOpened()){
		cout << "Video file error" << endl;
		vector<vector<pair<float,float>>> RaysRoi(0);
		return(RaysRoi);
	}
	vector<vector<pair<float,float>>> RaysRoi;

	int nFrames = video.get(CV_CAP_PROP_FRAME_COUNT) - 1;

	Mat frame0;
	video >> frame0;

	Mat frame1;

	map<pair<int,int>,vector<pair<float,float>>>  RaysMap;

	while(video.read(frame1)){

		for (int i = 0; i < frame0.rows; ++i){

			for (int j = 0; j < frame0.cols; ++j){

				pair<int,int> pixel = make_pair(i,j);

				ROI SupportRegion = CreateRoi(frame0,i,j,size);	

  				ROI WindowSearch = CreateRoi(frame1,i,j,(2*size + 1));	

				RaysMap[pixel].push_back( ComputeFlow( pixel, SupportRegion, WindowSearch,frame0,frame1, size, nFrames) );
			}
		}

		frame1 = frame0;
	}

	for (std::map<pair<int,int>,vector<pair<float,float>>>::iterator i = RaysMap.begin(); i != RaysMap.end(); ++i){
		RaysRoi.push_back( Normalize(i->second,SizeNorm) );
	}

	return (RaysRoi);
}

template<typename T>
vector<pair<float,float>> RaysExtractor<T>::Normalize(const vector<pair<float,float>> rays,const int SizeNorm){
	float rate = float(rays.size()) / float(SizeNorm) ; 
	vector<pair<float,float>> NormalizeRays;	

	float first,second,RemanentRate;
	float RemanentActual = 1.0;
	for (int i = 0,j = 0; i < SizeNorm; ++i)
	{
		first  		 = 0.0;
		second 		 = 0.0;
		RemanentRate = rate;

		while(RemanentRate > 0.0){
			if(RemanentRate >= RemanentActual){
				first         += RemanentActual * float(rays[j].first); 
				second        += RemanentActual * float(rays[j].second);
				RemanentRate  -= RemanentActual;
				RemanentActual = 1.0;	
				j++;						
			}
			else{
				first          += RemanentRate * float(rays[j].first);
				second         += RemanentRate * float(rays[j].second);
				RemanentActual -= RemanentRate;
				RemanentRate    = 0.0;
			}
		}

		NormalizeRays.push_back(make_pair(first,second));
	}

	return (NormalizeRays);
}