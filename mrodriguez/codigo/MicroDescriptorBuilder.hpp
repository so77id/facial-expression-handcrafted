#include <fstream>
#include <string>
#include "opencv2/opencv.hpp"
#include "RaysExtractor.hpp"

using namespace std;
using namespace cv;

template<typename T>
class MicroDescriptorBuilder
{
	private:
		ifstream inFile_;
		ofstream outFile_;
		RaysExtractor<T> RaysExtractor_;
		size_t SizeRaysUniverse_;
	public:
		MicroDescriptorBuilder(string&,string&);
		~MicroDescriptorBuilder();
		bool isGood();
		bool Build(const int,const int);
};

template<typename T>
MicroDescriptorBuilder<T>::MicroDescriptorBuilder(string &inFile, string &outFile){
	inFile_ .open(inFile);
	outFile_.open(outFile);
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
		vector<vector<pair<float,float>>> Rays = std::move(RaysExtractor_.Extract(Video,SupportRegionSize,SizeNorm));
			
		int ROI = 1;	
		for (std::vector<vector<pair<float,float>>>::iterator i = Rays.begin(); i != Rays.end(); ++i)
		{
			outFile_ << descount++ << " " << VideoId << " " << ROI;

			for (std::vector<pair<float,float>>::iterator j = i->begin(); j != i->end(); ++j)
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