#include <iostream>
#include <fstream>
#include <vector>
	#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;


float TransformLabels(int i){

	float dato = 0.0;

	switch (i){
		case 1:
			dato = -1.0;
			break;
		case 2:
			dato = -0.6;
			break;
		case 3:
			dato = -0.2;
			break;
		case 4:
			dato = 0.2;
			break;
		case 5:
			dato = 0.6;
			break;
		case 6:
			dato = 1.0;
			break;
	}
	return dato;
}


//archivo con macrodescriptores
//archivo con clases para video

int main(int argc, char const *argv[])
{
	if (argc < 5){
		cout << "Error en los argumentos" << endl;
		cout << "./Codigo <Macrodescriptores> <clases>" << endl;
		return (-1);
	}

	/*{

		int index;
		int buffer;
		int class_;
		int frames;
		string Buffer;

		ifstream infile(argv[1]);
		ifstream classfile(argv[2]);

		if (!infile.good() || !classfile.good())
		{
			cout << "Uno de los archivos falla" << endl;	
			return(-1);
		}
	
		vector<int> contador(7,0);

		int c = 0;
		while(!classfile.eof()){
			classfile >> Buffer >> index >> class_ >> frames;
			c++;
			if (c < 17) continue;
			contador[class_]++;
		}

		for (int i = 0; i < contador.size(); ++i)
		{
			cout << "Clase: "<< i << " --> " << contador[i] << endl;
		}

		infile.close();
		classfile.close();

	}*/


	ifstream DescriptorsFile(argv[1]);
	ifstream Classfile(argv[2]);

	ifstream DescriptorsFile_test(argv[3]);
	ifstream Classfile_test(argv[4]);


	if (!DescriptorsFile.good() || !Classfile.good() || !DescriptorsFile_test.good() || !Classfile_test.good())
	{
		cout << "Uno de los archivos falla" << endl;	
		return(-1);
	}

	int nClusters_;

	DescriptorsFile >> nClusters_;
	map<int,vector<int>> DescriptorsVector; //key indice del video
	map<int,int> classVector; //key indice del video

	int index;
	int buffer;
	int class_;
	int frames;
	string Buffer;


	while(!DescriptorsFile.eof()){
		DescriptorsFile >> index;
		for (int i = 0; i < nClusters_; ++i)
		{
			DescriptorsFile >> buffer;
			DescriptorsVector[index].push_back(buffer);
		}
	}


	while(!Classfile.eof()){
		Classfile >> Buffer >> index >> class_ >> frames;
		classVector[index] = class_;
	}


	Mat labelsMat(classVector.size(), 1, CV_32FC1);
	
	int i = 0;
	for (std::map<int,int>::iterator it = classVector.begin(); it != classVector.end(); ++it)
	{
		for (int j = 0; j < 1; ++j)
		{
			labelsMat.at<float>(i,j) = it->second;
		}
		i++;
	}


	i = 0;
	int j;
	Mat trainingDataMat(DescriptorsVector.size(), nClusters_, CV_32FC1);

	for (std::map<int,vector<int>>::iterator it_map = DescriptorsVector.begin(); it_map != DescriptorsVector.end(); ++it_map)
	{
		j = 0;
		for (std::vector<int>::iterator it_vector = it_map->second.begin(); it_vector != it_map->second.end(); ++it_vector)
		{
			trainingDataMat.at<float>(i,j) = *it_vector;
			j++;	
		}
		i++;	
	}



    for (int i = 0; i < trainingDataMat.rows; ++i)
    {
        for(int j = 0; j < trainingDataMat.cols; ++j)
        {
            cout << trainingDataMat.at<float>(i,j) << " ";
        }
        cout << endl;
    }

    cout << trainingDataMat.rows << endl;
    cout << labelsMat.rows << endl;



	CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);


    CvSVM SVM;
    
   
	// Use auto-training parameter grid search (ignore params manually
	// specified above)
	SVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);

//-----------------------------------------------------------------------

	map<int,vector<int>> DescriptorsVector_test; //key indice del video
	map<int,int> classVector_test; //key indice del video


	while(!DescriptorsFile_test.eof()){
		DescriptorsFile_test >> index;
		for (int i = 0; i < nClusters_; ++i)
		{
			DescriptorsFile_test >> buffer;
			DescriptorsVector_test[index].push_back(buffer);
		}
	}


	while(!Classfile_test.eof()){
		Classfile_test >> Buffer >> index >> class_ >> frames;
		classVector_test[index] = class_;
	}


	Mat labelsMat_test(classVector_test.size(), 1, CV_32FC1);
	
	for (std::map<int,int>::iterator it = classVector_test.begin(); it != classVector_test.end(); ++it)
	{
		for (int j = 0; j < 1; ++j)
		{
			labelsMat_test.at<float>(i,j) = it->second;
			cout << labelsMat_test.at<float>(i,j) << endl;
			
		}
		i++;
	}


	i = 0;
	Mat trainingDataMat_test(DescriptorsVector_test.size(), nClusters_, CV_32FC1);

	for (std::map<int,vector<int>>::iterator it_map = DescriptorsVector_test.begin(); it_map != DescriptorsVector_test.end(); ++it_map)
	{
		j = 0;
		for (std::vector<int>::iterator it_vector = it_map->second.begin(); it_vector != it_map->second.end(); ++it_vector)
		{
			trainingDataMat_test.at<float>(i,j) = *it_vector;
			j++;	
		}
		i++;	
	}



	for (int i = 0; i < trainingDataMat_test.rows; ++i)
	{
		cout << SVM.predict(trainingDataMat_test.row(i)) << " real: " << labelsMat_test.at<float>(i,0) << endl;
		cout << (SVM.predict(trainingDataMat_test.row(i)) == labelsMat_test.at<float>(i,0) ? "true" : "false") << endl;
	}


	return 0;
}