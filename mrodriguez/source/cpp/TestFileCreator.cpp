#include <fstream>
#include <iostream>
#include <string>
#include <limits>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


int main(int argc, char const *argv[])
{
	if(argc < 4){
		cout << "Error en los argumentos <clases por video> <archivo salida> <path de videos>" << endl;
		return(-1);
	}

	ifstream InFile(argv[1]);
	ofstream OutFile(argv[2]);
	string path(argv[3]);

	if(!InFile.good()){
		cout << "Archivo de entrada no existe" << endl;
		return(-1);
	}

	if(!OutFile.good()){
		cout << "Archivo de salida no existe" << endl;
		return(-1);
	}

	int min = std::numeric_limits<int>::max();
	int ind = 0;

	int num_video;
	int Class;

	for (int i = 1; i <= 168; ++i)
	{
		InFile >> num_video >> Class;


		string nombre("rigid_test");
		string extencion(".avi");
		string result = path + nombre + std::to_string(i) + extencion;

		VideoCapture video;
		video.open(result);

		if(!video.isOpened()){
			cout << result << endl;
			continue;
		}

		int frames = video.get(CV_CAP_PROP_FRAME_COUNT);

		result += " " + std::to_string(i) +" " + std::to_string(Class) + " " + std::to_string(frames);

		if(frames < min){
			min = frames;
			ind = i;
		}


		OutFile << result << endl;
	}

	cout << "Min: " << min << " Del video: " << ind << endl;

	return 0;
}
