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

/*
	typedef pair<int,int> pixel; // first -> es la coordenada X,  second -> es la coordenada Y
	typedef pair<pair<int,int>,int> voxel; // first -> pixel (con first -> X, second -> Y) ; second -> coordenada T
	typedef pair<float,float> RaySupport; //Rayo de soporte, unidad minima de un rayo de flujo (ray flux)
	typedef vector<RaySupport> RayFlux; //Rayo de flujo
	typedef vector<RayFlux> ListRaysFlux; // vector de rayos de flujo
*/



template<typename T>
class RaysExtractor
{
	private:
		T MSE(T, T);
		T SumMSE(const ROI&, const ROI&, const Mat&, const Mat &);
		pixel ComputeFlow(const ROI &,const ROI &, const Mat &, Mat &,const int);
	public:
		ListRaysFlux Extract(VideoCapture &,const int);
		RayFlux Normalize(const RayFlux,const int);
};


//Dado 2 valores de intensidad, calcula el error cuadratico entre ellos como  (i1 - i2)^2

template<typename T>
T RaysExtractor<T>::MSE(T intensity0, T intensity1){
	return (  (intensity0 - intensity1) * (intensity0 - intensity1)  );
}


//Funcion que dado la RS y una Seccion de WS (del mismo tamaño de RS) calcula el MSE  entre ambas regiones
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

//Calcula a que pixel se mueve la RS en WS
template<typename T>
pixel RaysExtractor<T>::ComputeFlow( const ROI &SR ,const ROI &WS, const Mat &ISR, Mat &IWS,const int size){

	vector<pair<T,pixel>> MSE_vector;
	using MSE_vector_iterator = typename std::vector<pair<T,pixel > >::iterator;


	for(int i = WS.RowsBegin(); i < WS.RowsEnd(); i++){
		for (int j = WS.ColsBegin(); j < WS.ColsEnd(); j++){

			ROI roiWS = CreateRoi(IWS,i,j,size);

			T mse = SumMSE(SR,roiWS,ISR,IWS);
			MSE_vector.push_back( std::make_pair(mse, make_pair(i,j) ) );
		}
	}


	T min = std::numeric_limits<T>::max();
	pixel point1;

	for (MSE_vector_iterator i = MSE_vector.begin(); i != MSE_vector.end(); ++i)
	{
		if(i->first <= min){
			min = i->first;
			point1 = i->second;
		}
	}

	//cout << (point0.first - point1.first) << endl;
	//std::make_pair( float(point0.first - point1.first) / float(nFrames), float(point0.second - point1.second) / float(nFrames) )

	return( point1 );
}


template<typename T>
ListRaysFlux RaysExtractor<T>::Extract(VideoCapture &video,const int SupportRegionSize){

	if(!video.isOpened()){
		cout << "Video file error" << endl;
		ListRaysFlux RaysRoi(0);
		return(RaysRoi);
	}
           //cuento la cantidad de frames del video
	int nFrames = video.get(CV_CAP_PROP_FRAME_COUNT) - 1;

	if(nFrames <= 1){
		cout << "La cantidad de frames de este video" << endl;
		ListRaysFlux RaysRoi(0);
	}

	Mat frame0;
	Mat frame1;

	//video >> frame0;
	video.read(frame0);

	map<pixel,pixel> MovimentMap;
	ListRaysFlux RaysRoi;

	for (int i = 0; i < frame0.rows; ++i)
	{
		for (int j = 0; j < frame0.cols; ++j)
		{
			pixel ActualPixel = make_pair(i,j);
			MovimentMap[ActualPixel] = ActualPixel;
		}
	}

	map<pixel,RayFlux>  RaysMap;

	while(video.read(frame1))
	{
		for (std::map<pixel,pixel>::iterator Map_it = MovimentMap.begin(); Map_it != MovimentMap.end(); ++Map_it)
		{
			pixel ActualPixel = Map_it->second;

			ROI SupportRegion = CreateRoi(frame0, ActualPixel.first, ActualPixel.second, SupportRegionSize);

			ROI WindowSearch = CreateRoi(frame1, ActualPixel.first, ActualPixel.second, (2*SupportRegionSize + 1));

			pixel  OptimalPixel =  ComputeFlow( SupportRegion, WindowSearch, frame0, frame1, SupportRegionSize );

			Map_it->second = OptimalPixel;

			RaySupport newRaySupport = std::make_pair( float(ActualPixel.first - OptimalPixel.first) / float(nFrames), float(ActualPixel.second - OptimalPixel.second) / float(nFrames) );

			RaysMap[Map_it->first].push_back(newRaySupport);
		}

		frame0 = frame1;
	}

	for (std::map<pixel,RayFlux>::iterator i = RaysMap.begin(); i != RaysMap.end(); ++i){
		//Cuando se normaliza directo
		//RaysRoi.push_back( Normalize(i->second,SizeNorm) );
		//CUando se normalzia en otro proceso
		RaysRoi.push_back( i->second );
	}

	return (RaysRoi);
}

template<typename T>
RayFlux RaysExtractor<T>::Normalize(const RayFlux rays,const int SizeNorm){
	float rate = float(rays.size()) / float(SizeNorm) ;
	RayFlux NormalizeRays;

	float first,second,RemanentRate;
	float RemanentActual = 1.0;
	for (int i = 0,j = 0; i < SizeNorm; ++i)
	{
		first  		 = 0.0;
		second 	 = 0.0;
		RemanentRate  = rate;

		while(RemanentRate > 0.0){
			if(RemanentRate >= RemanentActual){
				first                  += RemanentActual * float(rays[j].first);
				second             += RemanentActual * float(rays[j].second);
				RemanentRate  -= RemanentActual;
				RemanentActual = 1.0;
				j++;
			}
			else{
				first                  += RemanentRate * float(rays[j].first);
				second              += RemanentRate * float(rays[j].second);
				RemanentActual -= RemanentRate;
				RemanentRate    = 0.0;
			}
		}

		NormalizeRays.push_back(make_pair(first,second));
	}

	return (NormalizeRays);
}
//========================================================================
//=========================  Versiones antiguas de funciones   =========================
//========================================================================

/*

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
vector<vector<pair<float,float>>> RaysExtractor<T>::Extract(VideoCapture &video,const int SupportRegionSize, const int SizeNorm){

	if(!video.isOpened()){
		cout << "Video file error" << endl;
		vector<vector<pair<float,float>>> RaysRoi(0);
		return(RaysRoi);
	}
	vector<vector<pair<float,float>>> RaysRoi;

           //cuento la cantidad de frames del video
	int nFrames = video.get(CV_CAP_PROP_FRAME_COUNT) - 1;


	Mat frame0;
	video >> frame0;

	Mat frame1;


	map<pair<int,int>,vector<pair<float,float>>>  RaysMap;

	while(video.read(frame1)){

		for (int i = 0; i < frame0.rows; ++i){

			for (int j = 0; j < frame0.cols; ++j){

				pair<int,int> pixel = make_pair(i,j);

				ROI SupportRegion = CreateRoi(frame0,i,j,SupportRegionSize);

  				ROI WindowSearch = CreateRoi(frame1,i,j,(2*SupportRegionSize + 1));

				RaysMap[pixel].push_back( ComputeFlow( pixel, SupportRegion, WindowSearch,frame0,frame1, SupportRegionSize, nFrames) );
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
		second 	 = 0.0;
		RemanentRate  = rate;

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

*/


