#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "utility.hpp"
#include <cstdio>

using namespace std;
using namespace cv;


class kFoldCrossValidation
{
	private:
		ifstream kFoldConfigFile_; //Archivo con los datos con las rutas a cada una de las instancias de prueba
		ifstream MacroDescriptorFile_; //Archivo con los macrodescriptores
		ifstream LabelsFile_;
		string kFoldPath_;
		//Mat MacroDescriptors_; //Mat con los descriptores
		map<int,vector<int>> MacroDescriptors_; // Mapa con los macrodescriptores
		map<int,int> MacroDescriptorsLabels_;
		//Mat MacroDescriptorsLabels_; //Mat con las etiquetas de los descriptores
		int nClusters_;	//tamaño de los descriptores
		int kFolds_; //cantidad de kfolds
		map<int,pair<vector<int>,vector<int>>> kFoldInstance; //mapa indexado por numero de ejmeplo de kfold y tiene un
												        // par de vectores con los datos de entrenamiento y los de test
		CvSVMParams Params_; // Parametros del SVM

		vector<double> Accuracy_;    // vector de presiciones

	public:
		kFoldCrossValidation(string, string, string, CvSVMParams);
		//~kFoldCrossValidation();
		bool loadDescriptors();
		void buildConfusionMatrix();
		void runKfoldCrossValidation();
		double GetAccuracy();
};

kFoldCrossValidation::kFoldCrossValidation(string MacroDescriptorFile, string kFoldPath, string LabelsFile, CvSVMParams Params){
	kFoldPath_ = kFoldPath;
	kFoldConfigFile_.open(kFoldPath_ + "kFold_config.txt");
	MacroDescriptorFile_.open(MacroDescriptorFile);
	LabelsFile_.open(LabelsFile);
	Params_ = Params;
}

bool kFoldCrossValidation::loadDescriptors(){

	if( !kFoldConfigFile_.good() || !MacroDescriptorFile_.good() || !LabelsFile_.good()){
		cout << "Sali por aca" << endl;
		return(false);
	}

//-----------------------------------------Extraccion de macrodescriptores----------------------------------
	cout << "\t Cargando macrodescriptores" << endl;

	MacroDescriptorFile_ >>	nClusters_;
	cout << "\t\tClusters " << nClusters_ << endl;
	//map<int,vector<int>> MacroDescriptors_;
	int id, feature;

	while(!MacroDescriptorFile_.eof()){
		MacroDescriptorFile_ >> id;
		//cout << "id: " << id;
		for (int i = 0; i < nClusters_; ++i)
		{
			//cout << " " << feature;
			MacroDescriptorFile_ >> feature;

			MacroDescriptors_[id].push_back(feature);
		}
		//cout << endl;
	}


	/*MacroDescriptors_ =  Mat(MacrodescriptorsMap.size(), nClusters_, CV_32FC2);

	i = 0;
	for (std::map<int,vector<int>>::iterator it_map = MacrodescriptorsMap.begin(); it_map != MacrodescriptorsMap.end(); ++it_map)
	{
		j = 0;
		for (std::vector<int>::iterator it_vector = it_map->second.begin(); it_vector != it_map->second.end(); ++it_vector)
		{
			MacroDescriptors_.at<float>(i,j) = *it_vector;
			j++;
		}
		i++;
	}*/

//--------------------------------------------Extraccion de labels del archivo-----------------------------------------

	cout << "\t Cargando labels" << endl;

	int index, class_, frames;
	string buffer;
	//map<int,int> MacroDescriptorsLabels_;

	while(!LabelsFile_.eof()){
		LabelsFile_ >> buffer >> index >> class_ >> frames;
		//cout << buffer << " " << index << " " << class_ << " " << frames << endl;
		MacroDescriptorsLabels_[index] = class_;
		//cout << MacroDescriptorsLabels_[index] << endl;
	}


/*	MacroDescriptorsLabels_ = Mat(MacrodescriptorsLabelsMap.size(), 1, CV_32FC2);

	int i = 0;
	for (std::map<int,int>::iterator it = MacrodescriptorsLabelsMap.begin(); it != MacrodescriptorsLabelsMap.end(); ++it)
	{
		for (int j = 0; j < 1; ++j)
		{
			MacroDescriptorsLabels_.at<float>(i,j) = it->second;
		}
		i++;
	}
*/
//-------------------------------------------Carga de los datos de prueba----------------------------------------------

	cout << "\t Cargando kfolds" << endl;


	string train, test;
	string path;
	int video_id;

	kFoldConfigFile_ >> kFolds_;

	cout << kFolds_ << endl;

	Accuracy_ = vector<double>(kFolds_,0.0);

	for (int i = 0; i < kFolds_; ++i)
	{
		kFoldConfigFile_ >> id >> train >> test;
		cout << id << " " << train << " " << test << endl;

		ifstream trainFile( kFoldPath_ + train);
		ifstream testFile( kFoldPath_ + test);

		if(!trainFile.good() || !testFile.good()){
			//cout << "no abrio el archivo" << endl;
			return (false);
		}

		cout << "TrainFile: " << endl;
		while(!trainFile.eof()){
			trainFile >> path >> video_id >> class_ >> frames;
			//cout << "\t" << video_id << endl;
			kFoldInstance[i].first.push_back(video_id);
		}

		cout << "TestFile: " << endl;
		while(!testFile.eof()){
			testFile >> path >> video_id >> class_ >> frames;
			//cout << "\t" << video_id << endl;
			kFoldInstance[i].second.push_back(video_id);
		}
	}

	return (true);
}


void kFoldCrossValidation::runKfoldCrossValidation(){

	Mat *trainData, *trainDataLabels, *testData, *testDataLabels;


	for (int i = 0; i < kFolds_; ++i)
	{

		//cout << "Hola" << endl;
		cout << "Validacion cruzada numero: " << i +1 << endl;

		trainData 	  = new Mat(kFoldInstance[i].first.size(),nClusters_,CV_32F);
		trainDataLabels = new Mat(kFoldInstance[i].first.size(),1,CV_32F);
		testData 	  = new Mat(kFoldInstance[i].second.size(),nClusters_,CV_32F);
		testDataLabels  = new Mat(kFoldInstance[i].second.size(),1,CV_32F);

		int row = 0, col;
		for (std::vector<int>::iterator it = kFoldInstance[i].first.begin(); it != kFoldInstance[i].first.end(); ++it)
		{
			col = 0;
			//cout << "Label: "<< MacroDescriptorsLabels_[*it] << endl;
			trainDataLabels->at<float>(row,0) = MacroDescriptorsLabels_[*it];

			//cout << "Descriptor "<< row+1 << " : ";
			for (std::vector<int>::iterator it_descriptors = MacroDescriptors_[*it].begin(); it_descriptors != MacroDescriptors_[*it].end(); ++it_descriptors)
			{
				//cout << *it_descriptors << " ";
				trainData->at<float>(row,col) = *it_descriptors;
				col++;
			}
			//cout << endl;
			row++;
		}

		//cout << "adios" << endl << endl;


		//for(int i=0; i<trainDataLabels.rows; i++)
		//    for(int j=0; j<trainDataLabels.cols; j++)
        		//	printf("trainDataLabels(%d, %d) = %f \n", i, j, trainDataLabels.at<float>(i,j));


		cout << "\t entrenando" << endl;
		CvSVM SVM_;
		SVM_.train(*trainData, *trainDataLabels, Mat(), Mat(), Params_);

		cout << "\t cargando datos de train" << endl;
		row = 0;
		for (std::vector<int>::iterator it = kFoldInstance[i].second.begin(); it != kFoldInstance[i].second.end(); ++it)
		{
			col = 0;
			testDataLabels->at<float>(row,0) = MacroDescriptorsLabels_[*it];

			for (std::vector<int>::iterator it_descriptors = MacroDescriptors_[*it].begin(); it_descriptors != MacroDescriptors_[*it].end(); ++it_descriptors)
			{
				testData->at<float>(row,col) = *it_descriptors;
				col++;
			}
			row++;
		}



		cout << "\t prediciendo" << endl;
		int HitCounter = 0;
		for (int r = 0; r < testData->rows; ++r)
		{
			//
			if( testDataLabels->at<float>(r,0) == SVM_.predict(testData->row(r)) ){
				HitCounter++;
			}
		}

		Accuracy_[i] = (HitCounter * 1.0) / (testData->rows * 1.0);

		cout << "\t Accuracy: " << Accuracy_[i] << endl;

		//cout << "Hola" << endl;


		Mat *AuxtrainData, *AuxtrainDataLabels, *AuxtestData, *AuxtestDataLabels;

		AuxtrainData =  trainData;
		AuxtrainDataLabels = trainDataLabels;
		AuxtestData = testData;
		AuxtestDataLabels = testDataLabels;


		delete(AuxtrainData);
		delete(AuxtrainDataLabels);
		delete(AuxtestData);
		delete(AuxtestDataLabels);
	}

}


double kFoldCrossValidation::GetAccuracy(){

	double accuracy = 0.0;

	for (std::vector<double>::iterator it = Accuracy_.begin(); it != Accuracy_.end(); ++it)
	{
		accuracy += *it;
	}

	return (accuracy / (Accuracy_.size()*1.0));
}