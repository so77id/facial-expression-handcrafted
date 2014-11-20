#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"


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
    typedef ListMacrodescriptors::iterator ListMacrodescriptorIter; //iterador pa la lista de macrodescriptores
    typedef  ListLabels::iterator ListLabelIter; //iterador para la lista de labels

    typedef vector<int> Set; //vector con los indices de los videos incluidos en un cierto set
    typedef Set::iterator SetIterator //Iterador de set
    typedef pair<Set,Set> DataSet; //first son los datos de entrenamiento y second los datos de pruebas
    typedef map<int,DataSet> MapDataSet; // key es el numero de la prueba del k-fold y el value es el dataset de la prueba
    typedef MapDataSet::iterator MapDataSetIter; //Iterador del map de datasets;

    typedef vector<vector<float>> ConfusionMatrix; //contenedor de una matriz de confucion
*/


class MacroDescriptorBuilder
{
    private:
        cv::Mat Vocabulary;

    public:
        MacroDescriptorBuilder();
        //Dataset, filename in, filename out
        MapMacrodescriptors Run(DataSet&, VideoRayFlux&, const size_t, const TermCriteria&, const size_t, const size_t);
        MapMacrodescriptors Run(const string&,DataSet&, VideoRayFlux&, const size_t, const TermCriteria&, const size_t, const size_t);
        //Ncluster, tc, retries, flags
        //cv::Mat ExtractClusters(const size_t, const TermCriteria&, const size_t, const size_t);
        //ListMacrodescriptors BuildMacrodescriptors();
};

MacroDescriptorBuilder::MacroDescriptorBuilder(){}

MapMacrodescriptors MacroDescriptorBuilder::Run(const string &OutFileName,DataSet& DS, VideoRayFlux& VideosRays, const size_t nClusters, const TermCriteria& tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001), const size_t retries = 1, const size_t flags = KMEANS_PP_CENTERS)
{
    ofstream Out(OutFileName);
    if(!Out.good())
    {
        cout << "Error al abrir el archivo" << endl;
        return MapMacrodescriptors();
    }

    MapMacrodescriptors  MMacros = std::move(Run(DS, VideosRays, nClusters, tc, retries, flags));

    Out << nClusters;

    for (MapMacrodescriptors::iterator MMIter = MMacros.begin(); MMIter != MMacros.end(); ++MMIter)
    {
          Out << endl << MMIter->first;
          for (Macrodescriptor::iterator MIter = MMIter->second.begin(); MIter != MMIter->second.end(); ++MIter)
          {
              Out << " " << *MIter;
          }
    }

    return(MMacros);
}

MapMacrodescriptors MacroDescriptorBuilder::Run( DataSet& DS, VideoRayFlux& VideosRays, const size_t nClusters, const TermCriteria& tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001), const size_t retries = 1, const size_t flags = KMEANS_PP_CENTERS)
{
    int RaysSize = VideosRays.begin()->second.size();
    int i,j,id;
    int sizeDataSet = DS.first.size() + DS.second.size();
    cv::Mat TrainData(DS.first.size(),RaysSize,CV_32F);
    cv::Mat TestData(DS.second.size(),RaysSize,CV_32F);
//    cv::Mat Vocabulary;
    MapMacrodescriptors MMacros;

    //Creando matrix de train

    i = 0;
    for (SetIterator SIter = DS.first.begin(); SIter != DS.first.end(); ++SIter)
    {
        MMacros[*SIter] = Macrodescriptor(nClusters,0);

        for (ListRaysFlux::iterator LIter = VideosRays[*SIter].begin(); LIter != VideosRays[*SIter].end(); ++LIter)
        {
            j = 0;
            for(RayFlux::iterator RIter = LIter->begin(); RIter != LIter->end(); ++ RIter)
            {
                            TrainData.at<float>(i,j) = RIter->first; j++;
                            TrainData.at<float>(i,j) = RIter->second; j++;
            }
            i++;
        }
    }

    //Creando matrix de test

    i = 0;
    for (SetIterator SIter = DS.second.begin(); SIter != DS.second.end(); ++SIter)
    {
        MMacros[*SIter] = Macrodescriptor(nClusters,0);

        for (ListRaysFlux::iterator LIter = VideosRays[*SIter].begin(); LIter != VideosRays[*SIter].end(); ++LIter)
        {
            j = 0;
            for(RayFlux::iterator RIter = LIter->begin(); RIter != LIter->end(); ++ RIter)
            {
                            TestData.at<float>(i,j) = RIter->first; j++;
                            TestData.at<float>(i,j) = RIter->second; j++;
            }
            i++;
        }
    }

    //Extrayendo clusters

    BOWKMeansTrainer bowTrainer(nClusters, tc, retries,flags);
    bowTrainer.add(TrainData);
    Vocabulary = bowTrainer.cluster();

    //Match para el train

    cv::Ptr<cv::DescriptorMatcher> matcher = new cv::BFMatcher(cv::NORM_L2);
    matcher->add(std::vector<cv::Mat>(1, Vocabulary));

    vector<DMatch> matchesTrain;
    matcher->match(TrainData,matchesTrain);


    i = 0; // contador del descriptor actual
    for (SetIterator SIter = DS.first.begin(); SIter != DS.first.end(); ++SIter)
    {
        for (j = 0; j < VideosRays[*SIter].size(); ++j)
        {
            id = matchesTrain[i].trainIdx;
            MMacros[*SIter][id]++;
            i++;
        }
    }

    vector<DMatch> matchesTest;
    matcher->match(TestData,matchesTest);


    i = 0; // contador del descriptor actual
    for (SetIterator SIter = DS.second.begin(); SIter != DS.second.end(); ++SIter)
    {
        for (j = 0; j < VideosRays[*SIter].size(); ++j)
        {
            id = matchesTest[i].trainIdx;
            MMacros[*SIter][id]++;
            i++;
        }
    }

    return(MMacros);

}