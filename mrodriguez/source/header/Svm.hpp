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
using namespace utility;

/*
    typedef pair<int,int> pixel; // first -> es la coordenada X,  second -> es la coordenada Y
    typedef pair<pair<int,int>,int> voxel; // first -> pixel (con first -> X, second -> Y) ; second -> coordenada T

    typedef pair<float,float> RaySupport; //Rayo de soporte, unidad minima de un rayo de flujo (ray flux)
    typedef vector<RaySupport> RayFlux; //Rayo de flujo
    typedef vector<RayFlux> ListRaysFlux; // vector de rayos de flujo
    typedef map<pixel,RayFlux> MapRaysFlux; //Mapa para saber que rayo le pertence a que pixel
    typedef map<int, ListRaysFlux> VideoRayFlux; //Mapa que permite saber dado un video sus rayos

    typedef vector<int> Macrodescriptor; // Macrodescriptor
    typedef vector<Macrodescriptor> ListMacrodescriptors; //Vector de macrodescriptores
    typedef map<int,Macrodescriptor> MapMacrodescriptors; //Mapa de macrodescriptores

    typedef vector<int> ListLabels; //Lista de labels de los macrodescriptores
    typedef map<int,int> MapLabels; //Mapa id video, label
    typedef ListMacrodescriptors::iterator ListMacrodescriptorIter; //iterador pa la lista de macrodescriptores
    typedef  ListLabels::iterator ListLabelIter; //iterador para la lista de labels

    typedef vector<int> Set; //vector con los indices de los videos incluidos en un cierto set
    typedef Set::iterator SetIterator; //Iterador de set
    typedef pair<Set,Set> DataSet; //first son los datos de entrenamiento y second los datos de pruebas

    typedef map<int,DataSet> MapDataSet; // key es el numero de la prueba del k-fold y el value es el dataset de la prueba
    typedef MapDataSet::iterator MapDataSetIter; //Iterador del map de datasets;

    typedef vector<vector<float>> ConfusionMatrix; //contenedor de una matriz de confucion
*/


class Svm
{
    private:
        double Accuracy_;
        ConfusionMatrix CMatrixResults_;
    public:
        Svm();
        ~Svm();
        void Run(DataSet&, MapLabels&, MapMacrodescriptors&, const CvSVMParams&, const size_t );
        ConfusionMatrix GetConfusionMatrix();
        double GetAccuracy();
};

Svm::Svm(){}
Svm::~Svm(){}

void Svm::Run(DataSet& DS, MapLabels& Labels, MapMacrodescriptors& MMacrodescriptors, const CvSVMParams &Params, const size_t nClass = 6 )
{
    int MacroSize = MMacrodescriptors.begin()->second.size();
    size_t i,j;
    cv::Mat TrainData(DS.first.size(),MacroSize,CV_32F);
    cv::Mat TrainLabel(DS.first.size(),1,CV_32F);
    cv::Mat TestData(DS.second.size(),MacroSize,CV_32F);
    cv::Mat TestLabel(DS.second.size(),1,CV_32F);
    cv::Mat PredictedLabel(DS.second.size(),1,CV_32F);

    cout << "creando matriz de confusion" << endl;
    for (i = 0; i < nClass; ++i)
    {
        CMatrixResults_.push_back(std::move(vector<float>(nClass,0.0)) );
    }

    cout << "creando matriz de entrenamieno" << endl;
    i = 0;
    for (SetIterator SIter = DS.first.begin(); SIter != DS.first.end(); ++SIter)
    {
        TrainLabel.at<float>(i,0) = Labels[*SIter];

        j = 0;
        for (Macrodescriptor::iterator MMIter = MMacrodescriptors[*SIter].begin(); MMIter != MMacrodescriptors[*SIter].end(); ++MMIter)
        {
            TrainData.at<float>(i,j) = *MMIter;
            j++;
        }
        i++;
    }

    cout << "creando matriz de test" << endl;
    i = 0;
    for (SetIterator SIter = DS.second.begin(); SIter != DS.second.end(); ++SIter)
    {
        TestLabel.at<float>(i,0) = Labels[*SIter];

        j = 0;
        for (Macrodescriptor::iterator MMIter = MMacrodescriptors[*SIter].begin(); MMIter != MMacrodescriptors[*SIter].end(); ++MMIter)
        {
            TestData.at<float>(i,j) = *MMIter;
            j++;
        }
        i++;
    }

    cout << "entrenando el SVM" << endl;
    CvSVM SVM_;
    SVM_.train(TrainData, TrainLabel, Mat(), Mat(), Params);

    cout << "Pr./ediciendo" << endl;
    SVM_.predict(TestData,PredictedLabel);

    cout << "Calculando la matrix de confusion" << endl;

    for (int ii = 0; ii < TestLabel.rows; ++ii)
    {
        cout << "\t en el (i,j)" << (TestLabel.at<float>(0,ii) - 1) << " , " << (PredictedLabel.at<float>(0,ii) -1) << endl;
        CMatrixResults_[TestLabel.at<float>(0,ii)-1][PredictedLabel.at<float>(0,ii)-1]++;
    }

    cout << "Calculando accuracy"<< endl;

    PredictedLabel = (PredictedLabel == TestLabel) / 255;
    Accuracy_ = mean(PredictedLabel.col(0))[0];
    cout << "Accuracy: " << Accuracy_ << endl;
}


double Svm::GetAccuracy(){ return (Accuracy_); }
ConfusionMatrix Svm::GetConfusionMatrix() { return (CMatrixResults_); }