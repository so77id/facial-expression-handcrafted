#include <iostream>
#include "../header/MicroDescriptorBuilder.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
	if(argc < 5){
		cout << "Error en los argumentos" << endl;
		cout << " ./Codigo <Lista de entrada> <Lista de salida> <SRSize> <WSsize> <tipo de descritpor>" << endl;
		return (-1);
	}
	string in(argv[1]);
	string out(argv[2]);
	int DescriptorType = std::atoi(argv[5]);


	MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder(in,out,DescriptorType);
	MyMicroDescriptorBuilder.Build(atoi(argv[3]), true, atoi(argv[4]));
	//MyMicroDescriptorBuilder.Build(atoi(argv[3]),atoi(argv[4]));


	return 0;
}