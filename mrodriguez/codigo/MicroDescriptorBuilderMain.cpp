#include <iostream>
#include "MicroDescriptorBuilder.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
	if(argc < 3){
		cout << "Error en los argumentos" << endl;
		return (-1);
	}
	string in(argv[1]);
	string out(argv[2]);

	MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder(in,out);
	MyMicroDescriptorBuilder.Build();

	return 0;
}