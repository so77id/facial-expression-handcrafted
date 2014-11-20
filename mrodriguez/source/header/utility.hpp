#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


//CenterX => cols
//CenterY => Rows


namespace utility{

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

	vector<string> split(string ,string );
	pair<double,double> mean( RayFlux &);
	pair<double,double> StandarDeviation( RayFlux & rayo);
	float TransformLabels(int );

	namespace lbp {
		// templated functions
		template <typename _Tp>
		void OLBP_(const cv::Mat& src, cv::Mat& dst);

		template <typename _Tp>
		void ELBP_(const cv::Mat& src, cv::Mat& dst, int radius = 1, int neighbors = 8);

		template <typename _Tp>
		void VARLBP_(const cv::Mat& src, cv::Mat& dst, int radius = 1, int neighbors = 8);

		// wrapper functions
		void OLBP(const cv::Mat& src, cv::Mat& dst);
		void ELBP(const cv::Mat& src, cv::Mat& dst, int radius = 1, int neighbors = 8);
		void VARLBP(const cv::Mat& src, cv::Mat& dst, int radius = 1, int neighbors = 8);

		// cv::Mat return type functions
		cv::Mat OLBP(const cv::Mat& src);
		cv::Mat ELBP(const cv::Mat& src, int radius = 1, int neighbors = 8);
		cv::Mat VARLBP(const cv::Mat& src, int radius = 1, int neighbors = 8);

	}

	namespace ldn{
		template<typename _Tp>
		void LDN_(const cv::Mat& src, cv::Mat& dst, cv::Mat& mask);
		void LDN(const cv::Mat& src, cv::Mat &dst, cv::Mat& mask);
		cv::Mat LDN(const cv::Mat& src, cv::Mat& mask);
	}

	namespace mask {
		cv::Mat kirsch(const int);
	}
}

#endif