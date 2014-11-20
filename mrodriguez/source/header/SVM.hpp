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


class SVM
{
    private:
        float Accuracy_;
        ConfusionMatrix CMatrixResults_;
    public:
        SVM();
        ~SVM();
        void Run(DataSet&, MapLabels&, MapMacrodescriptors&, const CvSVMParams&, const int );
        ConfusionMatrix GetConfusionMatrix();
        double GetAccuracy();
};

SVM::SVM(){}
SVM::~SVM(){}

void SVM::Run(DataSet& DS, MapLabels& Labels, MapMacrodescriptors& MMacrodescriptors, const CvSVMParams &Params, const size_t nClass = 6 )
{
    int MacroSize = MapMacrodescriptors.begin()->second.size();
    size_t i,j;
    cv::Mat TrainData(DS.first.size(),MacroSize,CV_32F);
    cv::Mat TrainLabel(DS.first.size(),1,CV_32F);
    cv::Mat TestData(DS.second.size(),MacroSize,CV_32F);
    cv::Mat TestLabel(DS.second.size(),1,CV_32F);
    cv::Mat PredictedLabel(DS.second.size(),1,CV_32F);

    for (int i = 0; i < NClass; ++i)
    {
        CMatrixResults_.push_back(std::move(vector<float>(NClass,0.0)) );
    }

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

    CvSVM SVM_;
    SVM_.train(TrainData, TrainLabel, Mat(), Mat(), Params);

    SVM_.predict(TestData,PredictedLabel);

    for (i = 0; i < TestData.rows; ++i)
    {
        CMatrixResults_[TestData.at<float>(0,i)-1][PredictedLabel.at<float>(0,i)-1]++;
    }

    PredictedLabel = (PredictedLabel == TestData) / 255;
    Accuracy_ = mean(PredictedLabel.col(0))[0];
}


float SVM::GetAccuracy(){ return (Accuracy_); }
ConfusionMatrix SVM::GetConfusionMatrix() { return (CMatrixResults_); }