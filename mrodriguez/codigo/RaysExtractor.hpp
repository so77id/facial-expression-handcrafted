#include "opencv2/opencv.hpp"
#include <vector>
#include <utility>
#include <limits>
#include <cmath>


using namespace cv;
using namespace std;


template<typename T>
class RaysExtractor
{
	private:
		T MSE(T, T);
		T SumMSE(const Mat&, const Mat &);
		pair<int,int> ComputeFlow(const pair<int,int> &, const Mat &,const Mat &, const int);
	public:
		vector<vector<pair<int,int>>> Extract(VideoCapture &,const int);
};


template<typename T>
T RaysExtractor<T>::MSE(T intensity0, T intensity1){
	//cout << "intensidad: " << intensity0 - intensity1 << endl;
	return (pow(intensity0 - intensity1,2));
}


template<typename T>
T RaysExtractor<T>::SumMSE(const Mat& SupportRegion, const Mat& RoiWS){
	T sum = 0;
	for (size_t i = 0; i < SupportRegion.rows; ++i)
	{
		for (size_t j = 0; j < SupportRegion.cols; ++j)
		{
			sum += MSE(SupportRegion.at<T>(i,j), RoiWS.at<T>(i,j));
		}
	}

	return (sum);
}

//SR SupportRegion
//WS Windows Search
template<typename T>
pair<int,int> RaysExtractor<T>::ComputeFlow(const pair<int,int> &point0, const Mat &SR ,const Mat &WS,const int size){
	
	vector<pair<T,pair<int,int> > > MSE_vector;	
	using MSE_vector_iterator = typename std::vector<pair<T,pair<int,int> > >::iterator;
	

	for(int i = 0; i < WS.rows; i++){
		for (int j = 0; j < WS.cols; j++){
			//cout << "intensity: " << intensity << " roi.at(i,j): " << roi.at<T>(i,j) << endl;
			int d = (size - 1)/2;

			int x = (j - d) < 0 ? 0 : j - d;
			int y = (i - d) < 0 ? 0 : i - d;
			int w = (j + d) > WS.cols - 1 ? (WS.cols - 1 - j) + d : size;
			int h = (i + d) > WS.rows - 1 ? (WS.rows - 1 - i) + d : size;	
			

			Rect WSrect(x,y,w,h);
			

			Mat roiWS = WS(WSrect);

			T mse = SumMSE(SR,roiWS);
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

//	cout << "MSE minimo: " << (int) min << " point0: " << point0.first << "," << point0.second << " point1: " << point1.first << "," << point1.second << endl;	

	return( std::make_pair( abs(point0.first - point1.first), abs(point0.second - point1.second) ) );
}


template<typename T>
vector<vector<pair<int,int>>> RaysExtractor<T>::Extract(VideoCapture &video,const int size){
	vector<vector<pair<int,int>>> RaysRoi;

	if(!video.isOpened()){
		cout << "Video file error" << endl;
		return(RaysRoi);
	}

	Mat frame0;
	video >> frame0;

	Mat frame1;

	map<pair<int,int>,vector<pair<int,int> > >  RaysMap;

	int frameCount = 0;
	while(video.read(frame1)){
		frameCount++;

		//cout << "Computando el frame: " << frameCount << endl;
		//cout << "w x h" << frame0.rows << " x " << frame0.cols << endl;

		int d,x,y,w,h;

		for (int i = 0; i < frame0.rows; ++i)
		{
			for (int j = 0; j < frame0.cols; ++j)
			{	
//				cout << "Computando el flow del frame: " << frameCount << " En el pixel (i,j): (" << i << "," << j << ")" << endl;  
				pair<int,int> pixel = make_pair(i,j);


				d = (size - 1)/2;

				x = (j - d) < 0 ? 0 : j - d;
				y = (i - d) < 0 ? 0 : i - d;
				w = (j + d) > frame0.cols - 1 ? (frame0.cols - 1 - j) + d : size;
				h = (i + d) > frame0.rows - 1 ? (frame0.rows - 1 - i) + d : size;

				Rect SRroi(x,y,w,h);
				Mat SupportRegion = frame0(SRroi);	


				d = (size);
				x = (j - d) < 0 ? 0 : j - d;
				y = (i - d) < 0 ? 0 : i - d;
				w = (j + d) > frame1.cols - 1 ? (frame1.cols - 1 - j) + d : 2*size+1;
				h = (i + d) > frame1.rows - 1 ? (frame1.rows - 1 - i) + d : 2*size+1;

				Rect WSroi(x,y,w,h);
  				Mat WindowSearch = frame1(WSroi);	

//				cout << "x: " << roi.x << " y: " << roi.x << " x + width: " << roi.x + roi.width << " y + height: " << roi.y + roi.height << endl; 
//				cout << "Computando el Flow de " << i << " , "  << j << endl;
				RaysMap[pixel].push_back( ComputeFlow( pixel, SupportRegion, WindowSearch , size) );
//				cout << "DEJE de computar" << endl << endl;
			}
		}

		frame1 = frame0;
	}

//	cout << "coordenada: (i,j) --> rayos (dx1,dy1) , (dx2,dy2) , ... , (dxn,dyn)" << endl;
	for (std::map<pair<int,int>,vector<pair<int,int> > >::iterator i = RaysMap.begin(); i != RaysMap.end(); ++i)
	{
		RaysRoi.push_back( i->second );
		/*
		cout << "coordenada: " << "(" << i->first.first << "," << i->first.second << ") --> rayos: ";
		for (std::vector<pair<int,int> >::iterator it = i->second.begin(); it != i->second.end(); ++it)
		{
			cout << "(" << it->first << "," << it->second << ") , ";			
		}
		cout << endl;
		*/
	}


//	video.release();	

}