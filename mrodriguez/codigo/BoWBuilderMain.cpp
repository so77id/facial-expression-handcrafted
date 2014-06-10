#include <iostream>
#include "BoWBuilder.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
	if(argc < 4){
		cout << "Error en los argumentos" << endl;
		cout << "./Codigo <Lista de entrada> <Lista de salida> <Numero de Clusters>" << endl;
		return (-1);
	}
	string in(argv[1]);
	string out(argv[2]);

	int *frec = new int[atoi(argv[3])];
	for(int ix = 0; ix < atoi(argv[3]); ix++){
		frec[ix] = 0;
	}

	BoWBuilder MyBow(in,out,atoi(argv[3]));
	MyBow.LoadDescriptors();
	MyBow.ExtractClusters(KMEANS_RANDOM_CENTERS);

	int size = MyBow.GetSizeDescriptors();

	for (int i = 0; i < size; ++i)
	{
		int cluster = MyBow.Classify(MyBow.GetDescriptor(i), utility::EuclideanDistance);
		frec[cluster]++;
	}

	for(int ix = 0; ix < atoi(argv[3]); ix++){
		cout << "El cluster " << ix << " tiene: " << frec[ix] << endl;
	}


	return 0;
}