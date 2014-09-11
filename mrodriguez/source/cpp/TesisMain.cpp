#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "../header/MicroDescriptorBuilder.hpp"

using namespace std;

bool MicroDescriptorBuilderRun(const string RSVideoList, const string RSPathOut, vector<int> &RS){

    string ConfigFileName = RSPathOut + "Config_microdescriptors_no_norm.txt";
    ofstream ConfigFile(ConfigFileName);

    if(!ConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores no normalizdos en la ruta: " << RSPathOut << endl;
        return(false);
    }

    ConfigFile << RS.size() << endl;

    for (vector<int>::iterator i = RS.begin(); i != RS.end(); ++i)
    {
         string  FileName = "microdescriptors_" + std::to_string(*i) + ".txt";
         int RSsize = *i;

         MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder(RSVideoList,FileName);
         if (MyMicroDescriptorBuilder.Build(RSsize))
         {
            ConfigFile << FileName << endl;
         }
         else
        {
            cout << "Error al crear los microdescriptores no norm con el valor " << *i << endl;
        }
    }

    return (true);
}




int main(int argc, char const *argv[])
{
    if(argc < 2){
        cout << "Debes utilizar al menos un flag -RS, -N, -K o -h (para ayuda)" << endl;
        return (-1);
    }

    string RSVideoList;
    string RSPathOut;
    vector<int> RS;


    vector<string> N;


    vector<string> K;



    for (int i = 1; i < argc; ++i)
    {
        string Aux(argv[i]);
        if(Aux == "-h"){
            //Escribir como mandar los parametros de cada cosa
            return(1);
        }

        if(Aux == "-RS"){
            //<cantidad de pruebas> <video_list> <carpeta donde se alojan los ejemplos> <P1> <P2> ... <PN>
            if(argc <= i + 1){
                cout << "Los parametros ingresados no corresponden a esta opcion. Intente de nuevo" << endl;
                return(-1);
            }
            int NPruebas = std::atoi(argv[++i]);
            cout << NPruebas << endl;

            if(argc <= (i + 2 + NPruebas) ){
                cout << "Los parametros ingresados  no corresponden a esta opcion. Intente de nuevo" << endl;
                return(-1);
            }

            RSVideoList = string(argv[++i]);
            cout << RSVideoList << endl;

            RSPathOut = string(argv[++i]);
            cout << RSPathOut << endl;



            for (int j = 0; j < NPruebas; j++)
            {
                    int AuxInt = std::atoi(argv[++i]);
                    if(AuxInt <=  0){
                        cout << "Los parametros ingreados en los valores no corresponden a esta opcion (ingrese valores mayores que 0)" << endl;
                        return(-1);
                    }
                    cout << AuxInt << endl;
                    RS.push_back(AuxInt);
            }

            if (MicroDescriptorBuilderRun(RSVideoList,RSPathOut,RS) == true){
                cout << "Extraccion de microdescriptres completa" << endl;
            }
            else
            {
                cout << "Ubo un error en la extraccion de microdescriptores :(" << endl;
            }

        }
        if(Aux == "-N"){

        }
        if(Aux == "-K"){

        }


    }


    return 0;
}