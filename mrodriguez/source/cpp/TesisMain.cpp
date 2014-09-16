#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "../header/MicroDescriptorBuilder.hpp"
#include "../header/BoWBuilder.hpp"

using namespace std;

bool MicroDescriptorBuilderRun(const string RSVideoList, const string RSPathOut, vector<int> &RS){


    string ConfigFileName = RSPathOut + "Config_microdescriptors_no_norm.txt";
    ofstream ConfigFile(ConfigFileName);

    if(!ConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores no normalizdos en la ruta: " << RSPathOut << endl;
        return(false);
    }

    ConfigFile << RS.size();

    for (vector<int>::iterator i = RS.begin(); i != RS.end(); ++i)
    {
         string  FileName = RSPathOut + "microdescriptors_" + std::to_string(*i) + ".txt";
         int RSsize = *i;

         cout << "Comienza la extraccion de microdescriptores RSValue: " << RSsize << endl;
         MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder(RSVideoList,FileName);
         if (MyMicroDescriptorBuilder.Build(RSsize,false))
         {
            ConfigFile  << endl << FileName <<  " " << *i;
         }
         else
        {
            cout << "Error al crear los microdescriptores no norm con el valor " << *i << endl;
        }
    }

    ConfigFile.close();

    return (true);
}


bool MicroDescriptorNormalize(const string &NPathNoNorm, const string &NPathNorm, const int Nmin, const int Nmax, const int Nstep){

    ifstream NoNormConfigFile(NPathNoNorm);
    ofstream NormConfigFile(NPathNorm + "Config_microdescriptors_norm.txt");

    if(!NoNormConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores no normalizdos: " << NPathNoNorm << endl;
        return(false);
    }

    if(!NormConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores normalizdos en la ruta: " << NPathNorm << endl;
        return(false);
    }

    int NoNormFIlesSize;
    int RSValue;
    string NoNormFileName;
    string NormFIleName;


    NoNormConfigFile >> NoNormFIlesSize;



    NormConfigFile << (NoNormFIlesSize * (((Nmax - Nmin) / Nstep ) + 1 )) ;


    for (int i = 0; i < NoNormFIlesSize; ++i)
    {
        NoNormConfigFile >> NoNormFileName;
        NoNormConfigFile >> RSValue;

         for (int j = Nmin; j <= Nmax; j+=Nstep )
        {

            cout << "Comienza la Normalizacion  de microdescriptores RSValue: " << RSValue << "NValue: " << j << endl;

            NormFIleName = NPathNorm + "microdescriptors_" + std::to_string(RSValue) + "_" + std::to_string(j) + ".txt";
            MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder;
            if (MyMicroDescriptorBuilder.NormalizeMicroDescriptors(NoNormFileName, NormFIleName, j, false ) )
            {
                NormConfigFile  << endl << NormFIleName << " " << RSValue << " " << j;
            }
            else
            {
                cout << "Error al crear los microdescriptores norm con los valores RS:" << i << " N: " <<  j <<endl;
            }
        }
    }

    NormConfigFile.close();

    return(true);
}

//=======================================================================

bool BoWBuilderAll(const string &KPathNormFileName, const string &KPathMacro, const int Nmin, const int Nmax, const int Nstep){

    ifstream KNormConfigFile(KPathNormFileName);
    ofstream KMacroConfilgFile(KPathMacro + "Config_macrodescriptors.txt");

    if(!KNormConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores no normalizdos: " << KPathNormFileName << endl;
        return(false);
    }

    if(!KMacroConfilgFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores normalizdos en la ruta: " << KPathMacro << endl;
        return(false);
    }

    int MacroFilesSize, RSValue, NValue;
    string MacroFileName, MicroFileName;

    KNormConfigFile >> MacroFilesSize;

    KMacroConfilgFile << (MacroFilesSize * (((Nmax - Nmin) / Nstep ) + 1 ));

    for (int i = 0; i < MacroFilesSize; ++i)
    {
            KNormConfigFile >> MicroFileName >> RSValue >> NValue;
            for (int j = Nmin; j <= Nmax; j+=Nstep )
            {

                cout << "Comienza la creacion de macrodescriptores RSValue: " << RSValue << "NValue: " << NValue <<  " KValue: " << j << endl;

                 MacroFileName = KPathMacro + "macrodescriptors_" + std::to_string(RSValue) + "_" + std::to_string(NValue) + "_" + std::to_string(j) + ".txt";

                 BoWBuilder MyBow(MicroFileName,MacroFileName,j);
                 MyBow.LoadDescriptors();
                 MyBow.ExtractClusters();

                 //TERMINAR DESDE ACA REVISAR
                 if(MyBow.BuildMacroDescriptors())
                 {
                    KMacroConfilgFile  << endl << MacroFileName << " " << RSValue << " " << NValue << " " << j;
                 }
                 else
                 {
                    cout << "Error al crear los Error al crear los macrodescriptores con valor RS:" << RSValue << " N: " <<  NValue << " K: " << j << endl;
                 }
            }
    }
    KNormConfigFile.close();
    return(true);
}




//=======================================================================


int main(int argc, char const *argv[])
{
    if(argc < 2){
        cout << "Debes utilizar al menos un flag -RS, -N, -K o -h (para ayuda)" << endl;
        return (-1);
    }

    string RSVideoList;
    string RSPathOut;
    vector<int> RS;


   string NPathNoNorm;
   string NPathNorm;
   int Nmin,Nmax,Nstep;

   string KPathNormFileName;
   string KPathMacro;
   int Kmin, Kmax, Kstep;



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
                cout << "Los parametros ingresados no corresponden a esta opcion (-RS) . Intente de nuevo" << endl;
                return(-1);
            }

            int NPruebas = std::atoi(argv[++i]);
            cout << NPruebas << endl;

            if(argc <= (i + 2 + NPruebas) ){
                cout << "Los parametros ingresados  no corresponden a esta opcion (-RS). Intente de nuevo" << endl;
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
                cout << "hubo un error en la extraccion de microdescriptores no normalizados :(" << endl;
            }

        }
        if(Aux == "-N"){
            //<archivo configuracion no normalizado> <path normalizado> <min value> <max value> <step>
            if(argc <= i + 5){
                cout << "Los parametros ingresados no corresponden a esta opcion (-N). Intente de nuevo" << endl;
                return(-1);
            }


            NPathNoNorm = string(argv[++i]);
            NPathNorm = string(argv[++i]);
            Nmin = atoi(argv[++i]);
            Nmax = atoi(argv[++i]);
            Nstep = atoi(argv[++i]);

           if( MicroDescriptorNormalize(NPathNoNorm, NPathNorm, Nmin, Nmax, Nstep) ){
                cout << "Normalizacion de microdescriptres completa" << endl;
           }
           else{
                cout << "hubo un error en la normalizacion de microdescriptores :'(" << endl;
           }


        }
        if(Aux == "-K"){
             //<archivo configuracion Normalizado> <path macrodescritptores> <min value> <max value> <step>
            if(argc <= i + 5){
                cout << "Los parametros ingresados no corresponden a esta opcion (-K). Intente de nuevo" << endl;
                return(-1);
            }

            KPathNormFileName = string(argv[++i]);
            KPathMacro = string(argv[++i]);
            Kmin = atoi(argv[++i]);
            Kmax = atoi(argv[++i]);
            Kstep = atoi(argv[++i]);

           if( BoWBuilderAll(KPathNormFileName, KPathMacro, Kmin, Kmax, Kstep) ){
                cout << "Creacion de macrodescriptres completa" << endl;
           }
           else{
                cout << "hubo un error en la creacion de macrodescriptores :((" << endl;
           }
       }
    }

    return 0;
}