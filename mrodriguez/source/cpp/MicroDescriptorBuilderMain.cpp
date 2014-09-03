#include <iostream>
#include "../header/MicroDescriptorBuilder.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
	if(argc < 5){
		cout << "Error en los argumentos" << endl;
		cout << "./Codigo <Lista de entrada> <Lista de salida> <Support Region Size> <Normalization Size>" << endl;
		return (-1);
	}
	string in(argv[1]);
	string out(argv[2]);

	MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder(in,out);
	MyMicroDescriptorBuilder.Build(atoi(argv[3]),atoi(argv[4]));

	return 0;
}