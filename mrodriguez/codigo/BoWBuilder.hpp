#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "utility.hpp"


using namespace std;
using namespace cv;


class BoWBuilder
{
	private:
		ifstream inFile_;
		ofstream outFile_;
		int	SizeOfVector_;
		size_t  NumberOfClusters_;
		Mat Clusters_;
		Mat MicroDescriptors_;
	public:
		BoWBuilder(const string &,const string &,const size_t);
		~BoWBuilder();
		bool isGood();
		bool LoadDescriptors();
		Mat GetDescriptor(const int);
		int GetSizeDescriptors();
		bool ExtractClusters(int,int,TermCriteria);
		int Classify(const Mat &,float (*)(const Mat &, const Mat &));
};

BoWBuilder::BoWBuilder(const string &inFile,const string &outFile, const size_t NumberOfClusters){
	inFile_ .open(inFile);
	outFile_.open(outFile);
	NumberOfClusters_ = NumberOfClusters;
}

BoWBuilder::~BoWBuilder(){
	inFile_ .close();
	outFile_.close();
}

bool BoWBuilder::isGood(){
	return(inFile_.good() && outFile_.good());
}


bool BoWBuilder::LoadDescriptors(){
	if(! isGood() ) return (false);

	string buffer;
	getline(inFile_,buffer);

	SizeOfVector_ = std::stoi(buffer);

	cout << SizeOfVector_ << endl;
	int video_id, roi, ray_id;
	
	vector<vector<float>> VectorMicroDescriptors_;
	cout << "Leyendo archivo" << endl;
	while(! inFile_.eof()){
		getline(inFile_,buffer);

		
		std::vector<string> Vbuffer = utility::split(buffer," ");
		if(Vbuffer.size() < size_t(SizeOfVector_ + 3)) break;

		//cout << Vbuffer[0] << " " << Vbuffer[1] << " " << Vbuffer[2]<< endl;
		ray_id   = std::stoi(Vbuffer[0]);
		video_id = std::stoi(Vbuffer[1]);
		roi      = std::stoi(Vbuffer[2]);

		//cout << "Video ID: "<< video_id << " ROI:" << roi << " Descriptor: " << count++ << endl;
		

		vector<float> MicroDescriptor;
		for (int i = 3; i < (SizeOfVector_ + 3); ++i)
		{
			MicroDescriptor.push_back( std::stod(Vbuffer[i]));
		}
		VectorMicroDescriptors_.push_back(std::move(MicroDescriptor));
	}

	cout << "creando matriz de descriptores" << endl;
	Mat MatMicroDescriptors_(VectorMicroDescriptors_.size(), VectorMicroDescriptors_.at(0).size(), CV_32F);
	for(int i=0; i<MatMicroDescriptors_.rows; ++i)
     for(int j=0; j<MatMicroDescriptors_.cols; ++j)
          MatMicroDescriptors_.at<float>(i, j) = VectorMicroDescriptors_.at(i).at(j);

    MicroDescriptors_ = MatMicroDescriptors_;

    return (true);
}


Mat BoWBuilder::GetDescriptor(const int i){
	return(MicroDescriptors_.row(i));
}

int BoWBuilder::GetSizeDescriptors(){
	return (MicroDescriptors_.rows);
}

int BoWBuilder::Classify(const Mat &Ray, float (*cmp)(const Mat &,const Mat &)){
	float min   = -1.0;
	int dist    = -2;
	int cluster = -1;
	for (int i = 0; i < Clusters_.rows; ++i)
	{
		dist = cmp(Clusters_.row(i),Ray);
		if(dist >= min){
			cluster = i;
			min     = dist;
		}
	}

	return(cluster);
}

bool BoWBuilder::ExtractClusters(int retries=1, int flags=KMEANS_PP_CENTERS, TermCriteria tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001)){
	
    cout << "Creando BOW" << endl;  
    BOWKMeansTrainer bowTrainer(NumberOfClusters_,tc,retries,flags);
	bowTrainer.add(MicroDescriptors_);
	
	cout << "Extrayendo clusters" << endl;
	Clusters_ = bowTrainer.cluster(); 


	return (true);

}