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

	typedef vector<int> Macrodescriptor; // Macrodescriptor
	typedef vector<Macrodescriptor> ListMacrodescriptors; //Vector de macrodescriptores;
	typedef vector<int> ListLabels; //Lista de labels de los macrodescriptores
	typedef ListMacrodescriptors::iterator ListMacrodescriptorIter; //iterador pa la lista de macrodescriptores
 	typedef  ListLabels::iterator ListLabelIter; //iterador para la lista de labels
    	typedef vector<int> Set; //vector con los indices de los videos incluidos en un cierto set
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
}

#endif


/*



	class ROI
	{
		private:
			int X_;
			int Y_;
			int Height_;
			int Width_;
			int CenterX_;
			int CenterY_;

		public:
			ROI(const int X,const int Y,const int Width,const int Height,const int CenterX,const int CenterY){
				X_       = X;
				Y_       = Y;
				Height_  = Height;
				Width_   = Width;
				CenterX_ = CenterX;
				CenterY_ = CenterY;
			}
			ROI(){};

			int GetX_() const { return (X_); }
			int GetY_() const { return (Y_); }
			int GetWidth_() const { return (Width_); }
			int GetHeight_() const { return (Height_); }
			int ColsBegin() const{ return (X_); }
			int RowsBegin() const { return (Y_); }
			int ColsEnd() const { return (X_ + Width_); }
			int RowsEnd() const { return (Y_ + Height_); }
			int CenterX() const { return (CenterX_); }
			int CenterY() const { return (CenterY_); }
			void operator=(const ROI &tmp){
				X_      = tmp.GetX_();
				Y_      = tmp.GetY_();
				Height_ = tmp.GetHeight_();
				Width_  = tmp.GetWidth_();
				CenterX_ = tmp.CenterX();
				CenterY_ = tmp.CenterY();
			}
	};



	//Funcion para crear ROI
	//i -> rows
	//j -> cols
	Mat CreateROI(const Mat& I,const int i, const int j,const int size){ //size impar

		int d = (size - 1)/2;

		int x   = (j - d) < 0 ? 0 : j - d;
		int w   = 1 + (j - x) + ( (j + d) > I.cols - 1 ? (I.cols - 1 - j) : d );

		int y   = (i - d) < 0 ? 0 : i - d;
		int h   = 1 + (i - y) + ( (i + d) > I.rows - 1 ? (I.rows - 1 - i) : d );

		Rect roi(x,y,w,h);

		return I(roi);
	}


	ROI CreateRoi(const Mat& I,const int i, const int j,const int size){ //size impar

		int d = (size - 1)/2;

		int x   = (j - d) < 0 ? 0 : j - d;
		int w   = 1 + (j - x) + ( (j + d) > I.cols - 1 ? (I.cols - 1 - j) : d );

		int y   = (i - d) < 0 ? 0 : i - d;
		int h   = 1 + (i - y) + ( (i + d) > I.rows - 1 ? (I.rows - 1 - i) : d );

		ROI roi(x,y,w,h,i,j);

		return (roi);
	}



	float EuclideanDistance(const Mat & vec1,const Mat &vec2){
		float sum = 0;
		if(vec1.cols == vec2.cols && vec1.rows == vec2.rows && vec1.rows == 1){
			for (int i = 0; i < vec1.cols; ++i)
			{
				sum += (vec1.at<float>(0,i) - vec2.at<float>(0,i)) * (vec1.at<float>(0,i) - vec2.at<float>(0,i)) ;
			}
		}
		return (sum);
	}

	float ManhattanDistance(const Mat & vec1,const Mat &vec2){
		float sum = 0;
		if(vec1.cols == vec2.cols && vec1.rows == vec2.rows && vec1.rows == 1){
			for (int i = 0; i < vec1.cols; ++i)
			{
				sum += abs((vec1.at<float>(0,i) - vec2.at<float>(0,i)));
			}
		}
		return (sum);
	}

	float HammingDistance(const Mat & vec1,const Mat &vec2){
		float sum = 0;
		if(vec1.cols == vec2.cols && vec1.rows == vec2.rows && vec1.rows == 1){
			for (int i = 0; i < vec1.cols; ++i)
			{
				sum += vec1.at<float>(0,i) == vec2.at<float>(0,i) ? 0 : 1;
			}
		}
		return (sum);
	}

	float CosineDistance(const Mat & vec1,const Mat &vec2){
		float sum = 0;
		if(vec1.cols == vec2.cols && vec1.rows == vec2.rows && vec1.rows == 1){
			sum = vec1.dot(vec2) / (norm(vec1) * norm(vec2));
		}
		return (sum);
	}



int main(int argc, char const *argv[])
{
	Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    cout << "Rows: " << image.rows << endl;
    cout << "Cols: " << image.cols << endl;


	ROI roi;
    roi = CreateRoi(image,50,50,5);


    cout << roi.GetX_() << endl;
    cout << roi.GetY_() << endl;

    for (int i = roi.RowsBegin(); i < roi.RowsEnd(); ++i){
    	for (int j = roi.ColsBegin(); j < roi.ColsEnd(); ++j){
    		cout << (int)image.at<uchar>(i,j) << " ";
    	}cout << endl;
    }

//    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
//    imshow( "Display window", roi );

	waitKey(0);
	return 0;
}*/