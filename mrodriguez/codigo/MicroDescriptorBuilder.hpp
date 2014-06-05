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
		vector<vector<vector<pair<int,int>>>> RaysUniverse_;
		size_t SizeRaysUniverse_;
	public:
		MicroDescriptorBuilder(string&,string&);
		~MicroDescriptorBuilder();
		bool isGood();
		bool Build();
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
bool MicroDescriptorBuilder<T>::Build(){
	if(! isGood() ) return (false);

	string VideoPath;
	string VideoClass;
	int    nVideoFrames;

	while(!inFile_.eof()){
		inFile_ >> VideoPath >> VideoClass >> nVideoFrames;

		VideoCapture Video;
		Video.open(VideoPath);
		cout << "Extrayendo rayos del video " << VideoPath << endl;
		RaysUniverse_.push_back(RaysExtractor_.Extract(Video));

		Video.release();
	}

	cout << RaysUniverse_.size() << endl;
}

