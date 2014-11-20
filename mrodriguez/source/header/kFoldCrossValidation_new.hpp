#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "utility.hpp"
#include "Svm.hpp"
#include "MacroDescriptorBuilder.hpp"
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
    typedef map<int, MapMacrodescriptors> kFoldMapMacrodescriptors; // first-> indice del kfold, second -> Macrodescriptores de la instancia

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

class kFoldCrossValidation
{
    private:
        bool State;
        MapDataSet MDS;
        VideoRayFlux VideosRays;
        MapLabels VideoLabels;

        double Accuracy_;
        ConfusionMatrix CMatrixResults_;

        kFoldMapMacrodescriptors kMMacros;
    public:
        kFoldCrossValidation();
        ~kFoldCrossValidation();
        bool Load(const string&, const string&, const string&);
        void RunAll(const size_t, const CvSVMParams &, const size_t, const TermCriteria& , const size_t, const size_t);
        void RunMacroDescriptorBuilderkFold(const size_t , const TermCriteria& tc, const size_t, const size_t);
        void RunSVM(const CvSVMParams&, const size_t);
        double GetAccuracy();
        ConfusionMatrix GetConfusionMatrix();
};

kFoldCrossValidation::kFoldCrossValidation()
{
    State = false;
    Accuracy_ = 0.0;
}

kFoldCrossValidation::~kFoldCrossValidation(){}

double kFoldCrossValidation::GetAccuracy() { return(Accuracy_); }

ConfusionMatrix kFoldCrossValidation::GetConfusionMatrix() { return (CMatrixResults_); }

bool kFoldCrossValidation::Load(const string& MicroDescriptorFileName, const string& kFoldPath, const string& LabelsFileName){

    int video_id, video_class, kfold_id, frames;
    int RaySize, kFoldSize;
    int i;
    float dx,dy;
    string trainName,testName, buffer;
    RayFlux NewRay;

    ifstream kFoldConfigFile       (kFoldPath + "kFold_config.txt");
    ifstream MicroDescriptorFile (MicroDescriptorFileName);
    ifstream LabelFile                (LabelsFileName);
    ifstream trainFile, testFile;

    if(!kFoldConfigFile.good() || !MicroDescriptorFile.good() || !LabelFile.good())
    {
        cout << "Error al abrir alguno de los archivos" << endl;
        return(false);
    }
//Cargando Labels
    cout << "cargando labels" << endl;

    while(! LabelFile.eof())
    {
        LabelFile >> buffer >> video_id >> video_class >> frames;
        VideoLabels[video_id] = video_class;
    }


//Cargando rayos
    cout << "cargando rayos" << endl;

    getline(MicroDescriptorFile,buffer);
    RaySize = std::stoi(buffer);



    cout << "RaySize: " << RaySize << endl;
    while(! MicroDescriptorFile.eof() )
    {
        getline(MicroDescriptorFile,buffer);
        std::vector<string> Vbuffer = utility::split(buffer," ");

        //ray_id     = std::stoi(Vbuffer[0]);
        video_id  = std::stoi(Vbuffer[1]);
        //roi          = std::stoi(Vbuffer[2]);

//        MicroDescriptorFile >> ray_id >> video_id >> roi >> frames;

        NewRay.clear();
        //cout << "Cargando el rayo " << ray_id << " video_id: " << video_id << endl;

        for(i = 3; i < RaySize +3;)
        {
            dx = std::stof(Vbuffer[i++]);
            dy = std::stof(Vbuffer[i++]);
            NewRay.push_back(std::make_pair(dx,dy));
        }
/*
        for (i = 3; i < HalfSize; ++i)
        {
            MicroDescriptorFile >> dx >> dy;
            NewRay.push_back(make_pair(dx,dy));
        }
*/
        VideosRays[video_id].push_back(std::move(NewRay));
    }


//Cargando K folds

    cout << "cargando kfold" << endl;

    kFoldConfigFile >> kFoldSize;

    for (i = 0; i < kFoldSize; ++i)
    {
        kFoldConfigFile >> kfold_id >> trainName >> testName;
        trainFile.open(kFoldPath + trainName);
        testFile.open(kFoldPath + testName);

        if(!trainFile.good() || !testFile.good())
        {
            cout << "error en la lectura del kfold" << endl;
            return(false);
        }

        while(!trainFile.eof())
        {
            trainFile >> buffer >> video_id >> video_class >> frames;
            MDS[i].first.push_back(video_id);
        }

        while(!testFile.eof())
        {
            testFile >> buffer >> video_id >> video_class >> frames;
            MDS[i].second.push_back(video_id);
        }

        trainFile.close();
        testFile.close();

    }

    State = true;
    return(State);
}


void kFoldCrossValidation::RunAll(const size_t nClusters, const CvSVMParams &Params, const size_t nClass = 6, const TermCriteria& tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001), const size_t retries = 1, const size_t flags = KMEANS_PP_CENTERS)
{
    if(!State)
    {
        cout << "Los datos no se cargaron correctamente" << endl;
        return;
    }

    size_t i,j;

    RunMacroDescriptorBuilderkFold(nClusters,tc,retries,flags);
    RunSVM(Params,nClass);

    cout << "Accuracy: " << Accuracy_ << endl;

    for (i = 0; i < CMatrixResults_.size(); ++i)
    {
        for (j = 0; j < CMatrixResults_[i].size(); ++j)
        {
           cout << CMatrixResults_[i][j] << "\t" << endl;
        }
        cout << endl;
    }

}


void kFoldCrossValidation::RunMacroDescriptorBuilderkFold(const size_t nClusters, const TermCriteria& tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001), const size_t retries = 1, const size_t flags = KMEANS_PP_CENTERS)
{
    if(!State)
    {
        cout << "Los datos no se cargaron correctamente" << endl;
        return;
    }

    cout << "Corriendo el kfold de macro para:" << endl;

    int i = 0;
    for (MapDataSetIter MDIter = MDS.begin(); MDIter != MDS.end(); ++MDIter)
    {
        cout << "ejemplo: " << ++i << endl;
        MacroDescriptorBuilder MDB;
        kMMacros[MDIter->first] = std::move(MDB.Run(MDIter->second,VideosRays, nClusters, tc, retries, flags));
    }
}

void kFoldCrossValidation::RunSVM(const CvSVMParams &Params, const size_t nClass = 6 )
{
    size_t i,j;

    if(!State)
    {
        cout << "Los datos no se cargaron correctamente" << endl;
        return;
    }

    for (i = 0; i < nClass; ++i)
    {
        CMatrixResults_.push_back(std::move(vector<float>(nClass,0.0)) );
    }


    cout << "Corriendo el kfold de svm para:" << endl;
    int z = 0;
    for (MapDataSetIter MDIter = MDS.begin(); MDIter != MDS.end(); ++MDIter)
    {
        cout << "ejemplo: " << ++z << endl;
        Svm mySvm;
        mySvm.Run(MDIter->second, VideoLabels, kMMacros[MDIter->first], Params, nClass);
        Accuracy_ += mySvm.GetAccuracy();
        ConfusionMatrix tmp = std::move(mySvm.GetConfusionMatrix());

        for (i = 0; i < tmp.size(); ++i)
        {
            for (j = 0; j < tmp[i].size(); ++j)
            {
                CMatrixResults_[i][j] += tmp[i][j];
            }
        }
    }
}