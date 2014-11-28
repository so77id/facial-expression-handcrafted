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
        bool debug_;
        bool State;
        MapDataSet MDS;
        VideoRayFlux VideosRays;
        MapLabels VideoLabels;

        double Accuracy_;
        ConfusionMatrix CMatrixResults_;

        kFoldMapMacrodescriptors kMMacros;

    public:
        kFoldCrossValidation(bool);
        ~kFoldCrossValidation();

        bool LoadKfolds(const string&);
        bool LoadLabels(const string&);
        bool LoadMicroDescriptors(const string&);
        bool LoadMacroDescriptors(const string&);

        bool LoadAll(const string&, const string&, const string&);
        bool LoadForBuildMacros(const string&, const string&);
        bool LoadForSVM(const string&,const string&, const string&);

        void RunAll(const size_t, const CvSVMParams &, const size_t, const TermCriteria& , const size_t, const size_t);
        bool RunMacroDescriptorBuilderkFold(string&, const size_t , const TermCriteria& tc, const size_t, const size_t);
        void RunMacroDescriptorBuilderkFold(const size_t , const TermCriteria& tc, const size_t, const size_t);
        bool RunSVM(const CvSVMParams&, const size_t);
        double RunSVM(const string&, const CvSVMParams&, const size_t);

        double GetAccuracy();
        ConfusionMatrix GetConfusionMatrix();

        VideoRayFlux GetVideosRays(){ return(VideosRays);}
        MapDataSet GetMDS(){ return(MDS); }
};

kFoldCrossValidation::kFoldCrossValidation(bool debug = false)
{
    debug_ = debug;
    State = false;
    Accuracy_ = 0.0;
}

kFoldCrossValidation::~kFoldCrossValidation(){}

double kFoldCrossValidation::GetAccuracy() { return((Accuracy_ / (MDS.size()*1.0) ) * 100 ); }

ConfusionMatrix kFoldCrossValidation::GetConfusionMatrix() { return (CMatrixResults_); }


bool kFoldCrossValidation::LoadMacroDescriptors(const string& MacroDescriptrosConfigFileName)
{
    int nClusters, video_id, cluster, kfold_id, i;
    string buffer;
    Macrodescriptor NewMacro;
    MapMacrodescriptors NewMapMacro;
    ifstream MacroConfigFile(MacroDescriptrosConfigFileName);
    ifstream MacroFile;

    if(!MacroConfigFile.good())
    {
        cout << "Error al cargar el archivo con la configuracion de los macrodescriptores" << endl;
        return(false);
    }

    while(!MacroConfigFile.eof())
    {
        MacroConfigFile >> buffer >> kfold_id;

        //cout << "cargando: " << buffer << endl;

        MacroFile.open(buffer);
        if(!MacroFile.good())
        {
            cout << "Error al abrir el archivo " << buffer << endl;
            return(false);
        }

        NewMapMacro.clear();

        MacroFile >> nClusters;
        while(!MacroFile.eof())
        {
            NewMacro.clear();

            MacroFile >> video_id;
            //cout << "\t Cargando el video: " << video_id << endl;
            for (i = 0; i < nClusters; ++i)
            {
                MacroFile >> cluster;
                NewMacro.push_back(cluster);
            }
            NewMapMacro[video_id] = NewMacro;
        }
        kMMacros[kfold_id] = NewMapMacro;

        MacroFile.close();
    }

    return(true);
}

bool kFoldCrossValidation::LoadKfolds(const string& kFoldPath)
{
    int kFoldSize, i, video_id , kfold_id;
    string trainName,testName, buffer;

    ifstream kFoldConfigFile    (kFoldPath + "kFold_config.txt");
    ifstream trainFile, testFile;

    if(!kFoldConfigFile.good())
    {
        cout << "Error al abrir el archivo de configuracion de kfoldpath: " << kFoldPath << endl;
        return(false);
    }

    kFoldConfigFile >> kFoldSize;

    for (i = 0; i < kFoldSize; ++i)
    {
        kFoldConfigFile >> kfold_id >> trainName >> testName;
        //cout << "kfold: " << kfold_id << endl;

        trainFile.open(kFoldPath + trainName);
        testFile.open(kFoldPath + testName);

        if(!trainFile.good() || !testFile.good())
        {
            cout << "error en la lectura del kfold" << endl;
            return(false);
        }

        //cout << "Train: " << endl;
        while(!trainFile.eof())
        {
            getline(trainFile, buffer);
            //cout << buffer << endl;
            if(buffer.length() < 1) break;
            std::vector<string> Vbuffer = utility::split(buffer," ");

            video_id = std::stoi(Vbuffer[1]);

            MDS[kfold_id].first.push_back(video_id);
            //cout << video_id << " | ";
        }//cout << endl;

        //cout << "Test: " << endl;
        while(!testFile.eof())
        {
            getline(testFile, buffer);
            std::vector<string> Vbuffer = utility::split(buffer," ");

            video_id = std::stoi(Vbuffer[1]);

            MDS[kfold_id].second.push_back(video_id);
            //cout << video_id << " | ";
        }//cout << endl;

        trainFile.close();
        testFile.close();
    }

    return(true);
}

bool kFoldCrossValidation::LoadLabels(const string& LabelsFileName)
{
    int video_id, video_class, frames;
    string buffer;

    ifstream LabelFile(LabelsFileName);

    if(!LabelFile.good())
    {
        cout << "Error al abrir el archivo de labels: " << LabelsFileName << endl;
        return(false);
    }

    while(! LabelFile.eof())
    {
        LabelFile >> buffer >> video_id >> video_class >> frames;
        VideoLabels[video_id] = video_class;
    }

    return(true);
}

bool kFoldCrossValidation::LoadMicroDescriptors(const string& MicroDescriptorFileName)
{
    int RaySize, video_id, i;
    float dx,dy;
    string buffer;
    RayFlux NewRay;

    ifstream MicroDescriptorFile (MicroDescriptorFileName);

    if(!MicroDescriptorFile.good())
    {
        cout << "Error al abrir el archivo de microdescritores" << endl;
        return(false);
    }

    getline(MicroDescriptorFile,buffer);
    RaySize = std::stoi(buffer);

    //cout << "RaySize: " << RaySize << endl;
    while(! MicroDescriptorFile.eof() )
    {
        getline(MicroDescriptorFile,buffer);
        std::vector<string> Vbuffer = utility::split(buffer," ");

        video_id  = std::stoi(Vbuffer[1]);

        NewRay.clear();
        //cout << "Cargando el rayo " << ray_id << " video_id: " << video_id << endl;

        for(i = 3; i < RaySize +3;)
        {
            dx = std::stof(Vbuffer[i++]);
            dy = std::stof(Vbuffer[i++]);
            NewRay.push_back(std::make_pair(dx,dy));
        }

        VideosRays[video_id].push_back(std::move(NewRay));
    }

    return(true);
}


bool kFoldCrossValidation::LoadForBuildMacros(const string& MicroDescriptorFileName, const string& kFoldPath)
{
    //Cargando rayos
    if(debug_) cout << "cargando rayos" << endl;
    if(!LoadMicroDescriptors(MicroDescriptorFileName))
    {
        State = false;
        return(State);
    }
    //Cargando K folds
    if(debug_) cout << "cargando kfold" << endl;
    if(!LoadKfolds(kFoldPath))
    {
        State = false;
        return (State);
    }
    //Retorno
    State = true;
    return(State);
}

bool kFoldCrossValidation::LoadForSVM(const string& MacroDescriptrosConfigFileName,const string& kFoldPath, const string& LabelsFileName)
{
    //cargando Macro
    if(debug_)  cout << "cargando macrodescriptores" << endl;
    if(!LoadMacroDescriptors(MacroDescriptrosConfigFileName))
    {
        State = false;
        return(State);
    }
    //Cargando K folds
    if(debug_)  cout << "cargando kfold" << endl;
    if(!LoadKfolds(kFoldPath))
    {
        State = false;
        return (State);
    }
    //Cargando Labels
    if(debug_)  cout << "cargando labels" << endl;
    if(!LoadLabels(LabelsFileName))
    {
        State = false;
        return (State);
    }
    //Retorno
    State = true;
    return(State);
}

bool kFoldCrossValidation::LoadAll(const string& MicroDescriptorFileName, const string& kFoldPath, const string& LabelsFileName)
{
    //Cargando Labels
    if(debug_)  cout << "cargando labels" << endl;
    if(!LoadLabels(LabelsFileName))
    {
        State = false;
        return (State);
    }
    //Cargando rayos
    if(debug_) cout << "cargando rayos" << endl;
    if(!LoadMicroDescriptors(MicroDescriptorFileName))
    {
        State = false;
        return(State);
    }
    //Cargando K folds
    if(debug_) cout << "cargando kfold" << endl;
    if(!LoadKfolds(kFoldPath))
    {
        State = false;
        return (State);
    }
    //Retorno
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
    int sum;

    RunMacroDescriptorBuilderkFold(nClusters,tc,retries,flags);
    RunSVM(Params,nClass);

    cout << "Accuracy: " << ( Accuracy_  / MDS.size() ) << endl;

    cout << "Matriz de confusion" << endl;

     for (i = 0; i < CMatrixResults_.size(); ++i)
    {
        sum = std::accumulate(CMatrixResults_[i].begin(), CMatrixResults_[i].end(), 0.0);
        for (j = 0; j < CMatrixResults_[i].size(); ++j)
        {
           cout  << (j == 0 ? "" : ",") << ( sum > 0 ?  ((CMatrixResults_[i][j] / (sum*1.0)) * 100) : 0);
        }
        cout << endl;
    }
}

bool kFoldCrossValidation::RunMacroDescriptorBuilderkFold(string& MacroPath, const size_t nClusters, const TermCriteria& tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001), const size_t retries = 1, const size_t flags = KMEANS_PP_CENTERS)
{
    if(!State)
    {
        cout << "Los datos no se cargaron correctamente" << endl;
        return (false);
    }

    ofstream MacroConfigFile(MacroPath + "macro_kfold_config.txt");
    if(!MacroConfigFile.good())
    {
        cout << "No se pudo crear el archivo de configuracion de macros para kfold en la ruta: " << MacroPath << endl;
        return (false);
    }
    if(debug_) cout << "Corriendo el kfold de macro para:" << endl;

    int i = 0;
    string FileName = "";
    for (MapDataSetIter MDIter = MDS.begin(); MDIter != MDS.end(); ++MDIter)
    {
        if(debug_) cout << "ejemplo: " << ++i << endl;
        FileName = MacroPath + "Macro_kfold_" + to_string(MDIter->first) + ".txt";

        MacroDescriptorBuilder MDB(debug_);
        kMMacros[MDIter->first] = std::move(MDB.Run(FileName,MDIter->second,VideosRays, nClusters, tc, retries, flags));
        MacroConfigFile << (i == 1 ? "" : "\n") << FileName << " "  << MDIter->first;
    }

    MacroConfigFile.close();
    return(true);

}


void kFoldCrossValidation::RunMacroDescriptorBuilderkFold(const size_t nClusters, const TermCriteria& tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001), const size_t retries = 1, const size_t flags = KMEANS_PP_CENTERS)
{
    if(!State)
    {
        cout << "Los datos no se cargaron correctamente" << endl;
        return;
    }

    if(debug_) cout << "Corriendo el kfold de macro para:" << endl;

    int i = 0;
    for (MapDataSetIter MDIter = MDS.begin(); MDIter != MDS.end(); ++MDIter)
    {
        if(debug_) cout << "ejemplo: " << ++i << endl;
        MacroDescriptorBuilder MDB(debug_);
        kMMacros[MDIter->first] = std::move(MDB.Run(MDIter->second,VideosRays, nClusters, tc, retries, flags));
    }
}


double kFoldCrossValidation::RunSVM(const string& ConfusionMatrixFileName, const CvSVMParams& Params, const size_t nClass = 6)
{
    if(RunSVM(Params,nClass))
    {
        size_t i, j;
        int sum;

        ofstream out(ConfusionMatrixFileName);
        if(!out.good())
        {
            cout << "No se pudo abrir el archivo para guardar la matriz de confusion" << endl;
        }

        for (i = 0; i < CMatrixResults_.size(); ++i)
        {
            sum = std::accumulate(CMatrixResults_[i].begin(), CMatrixResults_[i].end(), 0.0);
            for (j = 0; j < CMatrixResults_[i].size(); ++j)
            {
               out  << (j == 0 ? "" : ",") << ( sum > 0 ?  ((CMatrixResults_[i][j] / (sum*1.0)) * 100) : 0);
            }
            out << endl;
        }

        out.close();
    }
    else
    {
        cout << "No se pudo correr el SVM" << endl;
    }

    return(GetAccuracy());
}


bool kFoldCrossValidation::RunSVM(const CvSVMParams &Params, const size_t nClass = 6 )
{
    size_t i,j;

    if(!State)
    {
        cout << "Los datos no se cargaron correctamente" << endl;
        return (false);
    }

    Accuracy_ = 0.0;
    CMatrixResults_.clear();
    for (i = 0; i < nClass; ++i)
    {
        CMatrixResults_.push_back(std::move(vector<float>(nClass,0.0)) );
    }


    if(debug_) cout << "Corriendo el kfold de svm para:" << endl;
    int z = 0;
    for (MapDataSetIter MDIter = MDS.begin(); MDIter != MDS.end(); ++MDIter)
    {
        if(debug_) cout << "ejemplo: " << ++z << endl;
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

    return(true);
}