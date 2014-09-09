#include <iostream>
#include "../header/MicroDescriptorBuilder.hpp"

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
    if(argc < 4){
        cout << "Error en los argumentos" << endl;
        cout << " ./Codigo <Archivo de rayos no normalizados> <Archivo de salida de rayos> <Tamaño de la normalizacion>" << endl;
        return (-1);
    }
    string in(argv[1]);
    string out(argv[2]);

    MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder;
    MyMicroDescriptorBuilder.NormalizeMicroDescriptors(in, out, std::atoi(argv[3]));
    //MyMicroDescriptorBuilder.Build(atoi(argv[3]),atoi(argv[4]));


    return 0;
}