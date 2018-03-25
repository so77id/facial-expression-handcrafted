#include "opencv2/opencv.hpp"
#include "../header/RaysExtractor.hpp"
#include <iostream>
#include <cstdlib>


using namespace cv;
using namespace std;



int main(int argc, char const *argv[])
{

	if(argc < 6) {
			cout << "Error en los argumentos" << endl;
			cout << "./programa <Video_path> <Size Support Region> <Size Windows Search>  <x> <y>" << endl;
			return (-1);
	}

	VideoCapture video;
	VideoCapture V1;
	video.open(argv[1]);

	//pixel Par = std::make_pair(std::atoi(argv[4]),std::atoi(argv[5]));



	RaysExtractor Extractor;
	map<pair<int,int>, vector<pair<float,float>>> RaysRoi = Extractor.Extract(video,std::atoi(argv[2]), std::atoi(argv[3]));


	/*int count = 0;
	for (std::map<pair<int,int>,vector<pair<float,float>>>::iterator i = RaysRoi.begin(); i != RaysRoi.end(); ++i)
	{
		count++;
		cout << "Rayo: " << count << " --> ";
		for (std::vector<pair<float,float>>::iterator j = i->second.begin(); j != i->second.end(); ++j)
		{
			cout << "(" << j->first << ","<< j->second << ") ";
		}cout << endl;
	}

	cout << RaysRoi.size() << endl;
	*/

	//TEST NORMALIZACION
/*	RaysExtractor<uchar> Extractor;
	vector<pair<float,float>> Rays;
	int tam ;
	int fin ;
	cin >> tam >> fin;

	for(int i = 0; i < tam; i++){
		float first,second;
		cin >> first >> second;
		Rays.push_back(make_pair(first,second));
	}

	vector<pair<float,float>> normRays = Extractor.Normalize(Rays,fin);

	float sum = 0;
	cout << endl;
	for (std::vector<pair<float,float>>::iterator i = normRays.begin(); i != normRays.end(); ++i)
	{
		sum += i->first;
		cout << i->first << " " << i->second << endl;
	}

	cout << sum << endl;
*/
	return 0;
}