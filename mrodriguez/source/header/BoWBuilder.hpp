#include <fstream>
#include <string>
#include <map>
#include <vector>
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
		map<pair<int,int>,vector<int>> VideoRoi_Rays_; // pair -> video_id, roi_id -> vector de rays id - 1
		map<int,vector<int>> VideoRoi_; // video_id -> vector de roi
		map<int,vector<int>> ClusterRays_; // id_cluster -> id_rays -1
	public:
		BoWBuilder(const string &,const string &,const size_t);
		~BoWBuilder();
		bool isGood();
		bool LoadDescriptors();
		Mat GetDescriptor(const int);
		int GetSizeDescriptors();
		bool ExtractClusters(int,int,TermCriteria);
		bool BuildMacroDescriptors();
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
	int c = 0;
	while(! inFile_.eof()){
		getline(inFile_,buffer);


		std::vector<string> Vbuffer = utility::split(buffer," ");
		if(Vbuffer.size() < size_t(SizeOfVector_ + 3)) break;

		//cout << Vbuffer[0] << " " << Vbuffer[1] << " " << Vbuffer[2]<< endl;
		ray_id   = std::stoi(Vbuffer[0]);
		video_id = std::stoi(Vbuffer[1]);
		roi      = std::stoi(Vbuffer[2]);

		if (video_id == 16) continue;

		pair<int,int> video_roi(video_id,roi);
		map<pair<int,int>,vector<int>>::iterator it_vrr = VideoRoi_Rays_.find(video_roi);

		if(it_vrr != VideoRoi_Rays_.end()){
			it_vrr->second.push_back(ray_id - 1);
		}
		else{
			VideoRoi_Rays_[video_roi].push_back(ray_id - 1);
			VideoRoi_[video_id].push_back(roi);
		}
		//cout << "Video ID: "<< video_id << " ROI:" << roi << " Descriptor: " << count++ << endl;
		vector<float> MicroDescriptor;
		for (int i = 3; i < (SizeOfVector_ + 3); ++i)
		{
			MicroDescriptor.push_back( std::stod(Vbuffer[i]));
		}
		VectorMicroDescriptors_.push_back(std::move(MicroDescriptor));
	}

	cout << "cantidad de rayos ingresados: " << c << endl;

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


bool BoWBuilder::ExtractClusters(int retries=1, int flags=KMEANS_PP_CENTERS, TermCriteria tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001)){

    cout << "Creando BOW" << endl;
    BOWKMeansTrainer bowTrainer(NumberOfClusters_, tc, retries,flags);
	bowTrainer.add(MicroDescriptors_);

	//for (int i = 0; i < MicroDescriptors_.rows; ++i)
//	{
	//	bowTrainer.add(MicroDescriptors_.row(i));
//	}

	cout << "Extrayendo clusters" << endl;
	Clusters_ = bowTrainer.cluster();


	/*cout << "----------------" << endl;
	for (int i = 0; i < Clusters_.rows; ++i)
	{
		for (int j = 0; j < Clusters_.cols; ++j)
		{
			cout << Clusters_.at<float>(i,j) << " ";
		}
		cout << endl;
	}
	cout << "----------------" << endl;
	*/
	cv::Ptr<cv::DescriptorMatcher > matcher = new cv::BFMatcher(cv::NORM_L2);
	matcher->add(std::vector<cv::Mat>(1, Clusters_));
	// matches
	vector<DMatch> matches;
	matcher->match(MicroDescriptors_,matches);

	int j= 0;
	for (vector<DMatch>::iterator it = matches.begin(); it != matches.end(); ++it)
	{
		int id = matches[j].trainIdx;

		ClusterRays_[id].push_back(j);

		j++;
	}
	cout << "Cantidad de rayos ingresados: " << j << endl;
	return (true);
}

bool BoWBuilder::BuildMacroDescriptors(){

	pair<int,int> RayRoi;
	vector<int> Rays;

	outFile_ << Clusters_.rows; // largo de los macro

	cout << "construyendo macro" << endl;




	for (std::map<int,vector<int>>::iterator video = VideoRoi_.begin(); video != VideoRoi_.end(); ++video)
	{

		vector<int> MacroDescriptor;
		RayRoi.first = video->first;
		for (std::vector<int>::iterator roi = video->second.begin(); roi != video->second.end(); ++roi)
		{

			RayRoi.second = *roi;
			Rays = VideoRoi_Rays_[RayRoi];

			vector<int> MacroDescriptorRoi(Clusters_.rows,0);
			int i = 0;
			//cout << "size: " << ClusterRays_.size() << endl;

			for(std::map<int,vector<int>>::iterator cluster = ClusterRays_.begin(); cluster != ClusterRays_.end() ; ++cluster)
			{

				for (std::vector<int>::iterator ray = cluster->second.begin(); ray != cluster->second.end(); ++ray)
				{
					if(std::binary_search(Rays.begin(),Rays.end(),*ray)){
						MacroDescriptorRoi[i]++;
					}
				}
				i++;
			}

			for (std::vector<int>::iterator it = MacroDescriptorRoi.begin(); it != MacroDescriptorRoi.end(); ++it)
			{
				MacroDescriptor.push_back(*it);
			}
		}

		outFile_ << endl << video->first;
		for (std::vector<int>::iterator it = MacroDescriptor.begin(); it != MacroDescriptor.end(); ++it)
		{
				outFile_ << " " << *it;
		}

	}

	return(true);
}