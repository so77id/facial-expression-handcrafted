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
		pair<int,int> ComputeFlow(const pair<int,int> &, const T,const Mat &);

	public:
		vector<vector<pair<int,int>>> Extract(VideoCapture &);
};


template<typename T>
T RaysExtractor<T>::MSE(T intensity0, T intensity1){
	//cout << "intensidad: " << intensity0 - intensity1 << endl;
	return (pow(intensity0 - intensity1,2));
}


template<typename T>
pair<int,int> RaysExtractor<T>::ComputeFlow(const pair<int,int> &point0, const T intensity ,const Mat &roi){
	
	vector<pair<T,pair<int,int> > > MSE_vector;	
	using MSE_vector_iterator = typename std::vector<pair<T,pair<int,int> > >::iterator;
	

	for(size_t i = 0; i < roi.rows; i++){
		for (int j = 0; j < roi.cols; j++)
		{
			//cout << "intensity: " << intensity << " roi.at(i,j): " << roi.at<T>(i,j) << endl;
			T mse = MSE(intensity,roi.at<T>(i,j));
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
vector<vector<pair<int,int>>> RaysExtractor<T>::Extract(VideoCapture &video){
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

		for (int i = 0; i < frame0.rows; ++i)
		{
			for (int j = 0; j < frame0.cols; ++j)
			{	
				//cout << "Computando el flow del frame: " << frameCount << " En el pixel (i,j): (" << i << "," << j << ")" << endl;  
				pair<int,int> pixel = make_pair(i,j);


				Rect roi = Rect( (i-2 >= 0 ? i-2 : 0) , (j-2 >= 0 ? j-2 : 0), (i-2+5 <= frame0.rows-1 ? 5 : 0 ), (j-2+5 <= frame0.cols-1 ? 5 : 0) );
				
//				cout << "x: " << roi.x << " y: " << roi.x << " x + width: " << roi.x + roi.width << " y + height: " << roi.y + roi.height << endl; 
 
				Mat RoiImg = frame1(roi);
				RaysMap[pixel].push_back( ComputeFlow( std::make_pair(i,j), frame0.at<uchar>(i,j), RoiImg) );
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