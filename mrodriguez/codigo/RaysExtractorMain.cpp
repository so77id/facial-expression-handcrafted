#include "opencv2/opencv.hpp"
#include "RaysExtractor.hpp"
#include <iostream>


using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{

	if(argc < 2) {
			cout << "Error en los argumentos" << endl;
			return (-1);
	}

	VideoCapture video;
	video.open(argv[1]);

	RaysExtractor<uchar> Extractor;
	vector<vector<pair<int,int>>> RaysRoi = Extractor.Extract(video,3);

	cout << RaysRoi.size() << endl;

	return 0;
}