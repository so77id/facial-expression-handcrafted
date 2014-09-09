#include <fstream>
#include <string>
#include "opencv2/opencv.hpp"
#include "RaysExtractor.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;
using namespace utility;

/*
	typedef pair<int,int> pixel; // first -> es la coordenada X,  second -> es la coordenada Y
	typedef pair<pair<int,int>,int> voxel; // first -> pixel (con first -> X, second -> Y) ; second -> coordenada T
	typedef pair<float,float> RaySupport; //Rayo de soporte, unidad minima de un rayo de flujo (ray flux)
	typedef vector<RaySupport> RayFlux; //Rayo de flujo
	typedef vector<RayFlux> ListRaysFlux; // vector de rayos de flujo
*/


template<typename T>
class MicroDescriptorBuilder
{
	private:
		ifstream inFile_;
		ofstream outFile_;
		RaysExtractor<T> RaysExtractor_;
		size_t SizeRaysUniverse_;
	public:
		MicroDescriptorBuilder(const string&, const string&);
		MicroDescriptorBuilder();
		~MicroDescriptorBuilder();
		bool isGood();
		bool Build(const int);
		bool NormalizeMicroDescriptors(const string&, const string&, const int)
};

template<typename T>
MicroDescriptorBuilder<T>::MicroDescriptorBuilder(const string &inFile,const string &outFile){
	inFile_ .open(inFile);
	outFile_.open(outFile);
	SizeRaysUniverse_ = 0;
}

template<typename T>
MicroDescriptorBuilder<T>::MicroDescriptorBuilder(){
	SizeRaysUniverse_ = 0;
}

template<typename T>
MicroDescriptorBuilder<T>::~MicroDescriptorBuilder(){
	inFile_ .close();
	outFile_.close();
}

template<typename T>
bool MicroDescriptorBuilder<T>::isGood(){
	return(inFile_.good() && outFile_.good());
}

template<typename T>
bool MicroDescriptorBuilder<T>::Build(const int SupportRegionSize){
	if(! isGood() ) return (false);

	string VideoPath;
	string VideoClass;
	int    VideoId;
	int    nVideoFrames;

	int descount = 1;
	//outFile_ << 2*SizeNorm << endl;


	while(!inFile_.eof()){

		inFile_ >> VideoPath >> VideoId >>VideoClass >> nVideoFrames;


		VideoCapture Video;
		Video.open(VideoPath);
		cout << "Extrayendo rayos del video " << VideoPath << endl;

		ListRaysFlux Rays = std::move(RaysExtractor_.Extract(Video,SupportRegionSize));

		int ROI = 1;
		for (ListRaysFlux::iterator i = Rays.begin(); i != Rays.end(); ++i)
		{
			outFile_ << descount++ << " " << VideoId << " " << ROI << " " << (nVideoFrames - 1)*2;

			for (RayFlux::iterator j = i->begin(); j != i->end(); ++j)
			{
				outFile_ << " " << j->first << " " << j->second;
			}

			outFile_ << endl;
		}

		SizeRaysUniverse_ += Rays.size();

		Video.release();
	}

	cout << "Rayos totales: " << SizeRaysUniverse_ << endl;
	return (true);
}

template<typename T>
bool MicroDescriptorBuilder<T>::NormalizeMicroDescriptors(const string &inFile,const string &outFile, const int SizeNorm){
	inFile_ .open(inFile);
	outFile_.open(outFile);

	if(! isGood() ) return (false);

	int Id, VideoId, ROI, RayFluxSize, HalfSize;
	float dx,dy;

	outFile_ << 2*SizeNorm << endl;

	while(! inFile_.eof() ){
		inFile_ >> Id >> VideoId >> ROI >> RayFluxSize;
		RayFlux NewRay;
		HalfSize = RayFluxSize / 2;
		for (int i = 0; i < RayFluxSize; ++i){
			inFile_ >> dx >> dy;
			NewRay.push_back(std::make_pair(dx,dy));
		}

		NewRay =  std::move(RaysExtractor_.Normalize(NewRay, SizeNorm));

		outFile_ << Id << " " << VideoId << " " << ROI;

		for (RayFlux::iterator j = NewRay.begin(); j != NewRay.end(); ++j)
		{
			outFile_ << " " << j->first << " " << j->second;
		}

		outFile_ << endl;
		SizeRaysUniverse_++;
	}
	cout << "Rayos totales: " << SizeRaysUniverse_ << endl;

	return(true);
}




/* VERSION ANTIGUA DE BUILD

template<typename T>
bool MicroDescriptorBuilder<T>::Build(const int SupportRegionSize, const int SizeNorm){
	if(! isGood() ) return (false);

	string VideoPath;
	string VideoClass;
	int    VideoId;
	int    nVideoFrames;

	int descount = 1;
	outFile_ << 2*SizeNorm << endl;


	while(!inFile_.eof()){

		inFile_ >> VideoPath >> VideoId >>VideoClass >> nVideoFrames;


		VideoCapture Video;
		Video.open(VideoPath);
		cout << "Extrayendo rayos del video " << VideoPath << endl;

		ListRaysFlux Rays = std::move(RaysExtractor_.Extract(Video,SupportRegionSize,SizeNorm));

		int ROI = 1;
		for (ListRaysFlux::iterator i = Rays.begin(); i != Rays.end(); ++i)
		{
			outFile_ << descount++ << " " << VideoId << " " << ROI;

			for (RayFlux::iterator j = i->begin(); j != i->end(); ++j)
			{
				outFile_ << " " << j->first << " " << j->second;
			}

			outFile_ << endl;
		}

		SizeRaysUniverse_ += Rays.size();

		Video.release();
	}

	cout << SizeRaysUniverse_ << endl;
	return (true);
}


*/