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
        bool debug_;
        cv::Mat Vocabulary;

    public:
        MacroDescriptorBuilder(const bool);
        //Dataset, filename in, filename out
        MapMacrodescriptors Run(DataSet&, VideoRayFlux&, const size_t, const TermCriteria&, const size_t, const size_t);
        MapMacrodescriptors Run(const string&,DataSet&, VideoRayFlux&, const size_t, const TermCriteria&, const size_t, const size_t);
        //Ncluster, tc, retries, flags
        //cv::Mat ExtractClusters(const size_t, const TermCriteria&, const size_t, const size_t);
        //ListMacrodescriptors BuildMacrodescriptors();
};

MacroDescriptorBuilder::MacroDescriptorBuilder(const bool debug = false){ debug_ = debug; }

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

    Out.close();

    return(MMacros);
}

MapMacrodescriptors MacroDescriptorBuilder::Run( DataSet& DS, VideoRayFlux& VideosRays, const size_t nClusters, const TermCriteria& tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001), const size_t retries = 1, const size_t flags = KMEANS_PP_CENTERS)
{

    size_t RaysSize = VideosRays.begin()->second.begin()->size()*2;
    size_t TrainSize = 0, TestSize = 0;


    //Calculando tamaño de las Mat par Train y Test
    for (SetIterator SIter = DS.first.begin(); SIter != DS.first.end(); ++SIter)
    {
        TrainSize += VideosRays[*SIter].size();
    }

    for (SetIterator SIter = DS.second.begin(); SIter != DS.second.end(); ++SIter)
    {
        TestSize += VideosRays[*SIter].size();
    }

    size_t i,j,id;

    cv::Mat TrainData(TrainSize,RaysSize,CV_32F);
    cv::Mat TestData(TestSize,RaysSize,CV_32F);

    MapMacrodescriptors MMacros;

    if(debug_) cout << "raysize: " << RaysSize << endl;
    if(debug_) cout << "RayCount" << TestSize + TrainSize << endl;

    //LLenando la Mat de Train
    if(debug_) cout << "Creando matrix de train" << endl;

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

    //LLennado a Mat de Test
    if(debug_) cout << "creadon matrix de test " << endl;
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
    if(debug_) cout << "Extrayendo clusters" << endl;

        BOWKMeansTrainer bowTrainer(nClusters, tc, retries,flags);
        bowTrainer.add(TrainData);
        Vocabulary = bowTrainer.cluster();

    //Match para el train
    if(debug_) cout << "Matcheando" << endl;

        cv::Ptr<cv::DescriptorMatcher> matcher = new cv::BFMatcher(cv::NORM_L2);
        matcher->add(std::vector<cv::Mat>(1, Vocabulary));

        vector<DMatch> matchesTrain;
        matcher->match(TrainData,matchesTrain);

    if(debug_) cout << "Creando descriptores de entrenamiento" << endl;

    //Creando Macrodescriptores de entrenamiento
    i = 0; // contador del descriptor actual
    for (SetIterator SIter = DS.first.begin(); SIter != DS.first.end(); ++SIter)
    {
        for (j = 0; j < VideosRays[*SIter].size(); ++j)
        {

            id = matchesTrain[i].trainIdx;
            MMacros[*SIter][id]++;
            i++;
        }
        //Comprobando que la suma de los descritpores sea igual a la cantidad de rayos
        size_t sum = std::accumulate(MMacros[*SIter].begin(),MMacros[*SIter].end(),0);

        if( VideosRays[*SIter].size() != sum )
        {
            cout << "EL macrodescriptor del video: " << *SIter << " No cumple con la regla de tamaño, Cantidad de rayos: " << VideosRays[*SIter].size() << ", Suma del histograma: " << sum << endl;
        }

    }

    vector<DMatch> matchesTest;
    matcher->match(TestData,matchesTest);

    if(debug_) cout << "Creando descriptores de test" << endl;

    i = 0; // contador del descriptor actual
    for (SetIterator SIter = DS.second.begin(); SIter != DS.second.end(); ++SIter)
    {
        for (j = 0; j < VideosRays[*SIter].size(); ++j)
        {
            id = matchesTest[i].trainIdx;
            MMacros[*SIter][id]++;
            i++;
        }

        size_t sum = std::accumulate(MMacros[*SIter].begin(),MMacros[*SIter].end(),0);

        if( VideosRays[*SIter].size() != sum )
        {
            cout << "EL macrodescriptor del video: " << *SIter << " No cumple con la regla de tamaño, Cantidad de rayos: " << VideosRays[*SIter].size() << ", Suma del histograma: " << sum << endl;
        }

    }

    return(MMacros);

}