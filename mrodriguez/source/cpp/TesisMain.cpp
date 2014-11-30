#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "../header/MicroDescriptorBuilder.hpp"
#include "../header/kFoldCrossValidation_new.hpp"

using namespace std;

bool MicroDescriptorBuilderRun(const string RSVideoList, const string RSPathOut, vector<int> &RS, vector<int> &WS){


    int sum = 0;
    string ConfigFileName = RSPathOut + "Config_microdescriptors_no_norm.txt";
    ofstream ConfigFile(ConfigFileName);

    if(!ConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores no normalizdos en la ruta: " << RSPathOut << endl;
        return(false);
    }

    for (vector<int>::iterator i = RS.begin(); i != RS.end(); ++i)
    {
        for (std::vector<int>::iterator j = WS.begin(); j != WS.end(); ++j)
        {
             if((2*(*i) +1) <= *j) sum++;
        }
    }

    ConfigFile << sum;

    for (vector<int>::iterator i = RS.begin(); i != RS.end(); ++i)
    {
        for (std::vector<int>::iterator j = WS.begin(); j != WS.end(); ++j)
        {
            if((2*(*i) +1) > *j)
            {
                cout << "Se descarta la extracion con los parametros RS: " << *i << " WS: " << *j << endl;
                continue;
            }

             string  FileName = RSPathOut + "microdescriptors_" + std::to_string(*i) + "_" +std::to_string(*j) + ".txt";
             int RSsize = *i;
             int WSsize = *j;

             cout << "Comienza la extraccion de microdescriptores RSValue: " << RSsize << " WSValue: "<<  WSsize << endl;
             MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder(RSVideoList,FileName);
             if (MyMicroDescriptorBuilder.Build(RSsize,false,WSsize))
             {
                ConfigFile  << endl << FileName <<  " " << *i << " " << *j;
             }
             else
            {
                cout << "Error al crear los microdescriptores no norm con el valor " << *i << endl;
            }
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
    int WSValue;
    string NoNormFileName;
    string NormFIleName;


    NoNormConfigFile >> NoNormFIlesSize;



    NormConfigFile << (NoNormFIlesSize * (((Nmax - Nmin) / Nstep ) + 1 )) ;


    for (int i = 0; i < NoNormFIlesSize; ++i)
    {
        NoNormConfigFile >> NoNormFileName;
        NoNormConfigFile >> RSValue;
        NoNormConfigFile >> WSValue;

         for (int j = Nmin; j <= Nmax; j+=Nstep )
        {

            cout << "Comienza la Normalizacion  de microdescriptores RSValue: " << RSValue << " WSValue: " << WSValue << " NValue: " << j << endl;

            NormFIleName = NPathNorm + "microdescriptors_" + std::to_string(RSValue) + "_" + std::to_string(WSValue) + "_" + std::to_string(j) + ".txt";
            MicroDescriptorBuilder<uchar> MyMicroDescriptorBuilder;
            if (MyMicroDescriptorBuilder.NormalizeMicroDescriptors(NoNormFileName, NormFIleName, j, false ) )
            {
                NormConfigFile  << endl << NormFIleName << " " << RSValue << " " << WSValue << " " << j;
            }
            else
            {
                cout << "Error al crear los microdescriptores norm con los valores RS:" << RSValue << " WS: " << WSValue << " N: " <<  j <<endl;
            }
        }
    }

    NormConfigFile.close();

    return(true);
}

//=======================================================================

bool MacroDescriptorBuilder(const string &KPathNormFileName, const string &KPathMacro, const string& path_kfold, const int Nmin, const int Nmax, const int Nstep){

    ifstream KNormConfigFile(KPathNormFileName);
    ofstream KMacroConfilgFile(KPathMacro + "Config_macrodescriptors.txt");


    //BOVW params
    TermCriteria tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001);
    size_t retries = 1;
    size_t flags = KMEANS_PP_CENTERS;

    if(!KNormConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores no normalizdos: " << KPathNormFileName << endl;
        return(false);
    }

    if(!KMacroConfilgFile.good()){
        cout << "No se logro abrir el archivo de configuracion de microdescriptores normalizdos en la ruta: " << KPathMacro << endl;
        return(false);
    }

    int MacroFilesSize, RSValue, WSValue, NValue;
    string MacroFoldName, MicroFileName, mkdirComand;

    KNormConfigFile >> MacroFilesSize;

    KMacroConfilgFile << (MacroFilesSize * (((Nmax - Nmin) / Nstep ) + 1 ));

    for (int i = 0; i < MacroFilesSize; ++i)
    {
            KNormConfigFile >> MicroFileName >> RSValue >> WSValue >> NValue;
            for (int j = Nmin; j <= Nmax; j+=Nstep )
            {

                cout << "Comienza la creacion de macrodescriptores RSValue: " << RSValue  << " WSValue: " << WSValue  <<  " NValue: "<< NValue <<  " KValue: " << j << endl;

                 MacroFoldName = KPathMacro + "macrodescriptors_" + std::to_string(RSValue) + "_" + std::to_string(WSValue) + "_" + std::to_string(NValue) + "_" + std::to_string(j) + "/";

                 mkdirComand = "mkdir " + MacroFoldName;

                 if(system(mkdirComand.c_str()) )
                 {
                    cout << "No se pudo crear la carpeta " << MacroFoldName << endl;
                    return(false);
                 }

                 kFoldCrossValidation myKFCV;
                 myKFCV.LoadForBuildMacros(MicroFileName,path_kfold);


                 if(myKFCV.RunMacroDescriptorBuilderkFold(MacroFoldName,j , tc , retries , flags ))
                 {
                    KMacroConfilgFile  << endl << MacroFoldName << " " << RSValue << " " << WSValue << " " << NValue << " " << j;
                 }
                 else
                 {
                    cout << "Error al crear los Error al crear los macrodescriptores con valor RS:" << RSValue  << " WS: " << WSValue << " N: " <<  NValue << " K: " << j << endl;
                 }
            }
    }
    KMacroConfilgFile.close();
    KNormConfigFile.close();
    return(true);
}

//=======================================================================

bool  SVMkFoldCrossValidation(const string& SVMMacroFoldConfig, const string& SVMPathResult, const string& SVMvideoList, const string& SVMkFoldPath){

    string CMatrixPath = SVMPathResult + "ConfusionMatrix/";
    string mkdir = "mkdir " + CMatrixPath;

    if(system(mkdir.c_str()))
    {
        cout << "No se puede crear la carpeta de matrices de confusion en: " << SVMPathResult << endl;
        return (false);
    }

    ifstream MacroFoldConfigFile(SVMMacroFoldConfig);
    ofstream AccuracyFile(SVMPathResult + "Accuracy.csv");
    ofstream CMConfigFile(CMatrixPath + "Config_CMatrix.txt");

    if(!MacroFoldConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de la carpeta de macrodescriptores" << SVMMacroFoldConfig << endl;
        return(false);
    }

    if(!AccuracyFile.good()){
        cout << "No se logro abrir el archivo de configuracion de resultados en la ruta: " << SVMPathResult << endl;
        return(false);
    }


    if(!CMConfigFile.good()){
        cout << "No se logro abrir el archivo de configuracion de los archivos CSV en la ruta: " << CMatrixPath << endl;
        return(false);
    }

    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF;
    params.gamma = pow(2,-20); //2^-x ---> x elevado
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);
   //variar gamma 0 -> -50
   // variar C 10^-6 -> 10^6 /// 0 -> 2 ... steap 0.1     ---- 0 -> 10 ... step 1 --- 10 -> 2000 ... step 10
    int MacroFilesSize, RSValue, WSValue, NValue, KValue;
    double Accuracy;
    string MacroFoldName, CMFileName;

    MacroFoldConfigFile >> MacroFilesSize;

    for (int i = 0; i < MacroFilesSize; ++i)
    {
        MacroFoldConfigFile >> MacroFoldName >> RSValue >> WSValue >> NValue >> KValue;
        //cout << "Leyendo Macrodescritproes: " <<MacroFoldName << " " << RSValue << " "  << WSValue << " "  << NValue << " "  << KValue << endl;

        CMFileName = "CM_" + std::to_string(RSValue) + "_" + std::to_string(WSValue) + "_" + std::to_string(NValue) + "_" + std::to_string(KValue) + ".csv";

        kFoldCrossValidation myKFCV;
        //cout << "LoadnForSVM: " << endl;
        myKFCV.LoadForSVM(MacroFoldName + "macro_kfold_config.txt",SVMkFoldPath, SVMvideoList);
        //cout << "Obteniendo Accuracy: "<< endl;
        Accuracy = myKFCV.kFoldCrossValidation::RunSVM(CMatrixPath + CMFileName,params, 6);

        cout << "Accuracy: " << Accuracy << " RSValue: " << RSValue  << " WSValue: " << WSValue  <<  " NValue: "<< NValue <<  " KValue: " << KValue << endl;
       AccuracyFile << (i == 0 ? "" : "\n") << RSValue << "\t" << WSValue << "\t" << NValue << "\t" << KValue << "\t" << Accuracy;
        CMConfigFile << (i == 0 ? "" : "\n") << CMatrixPath + CMFileName;
    }

    CMConfigFile.close();
    AccuracyFile.close();

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
    vector<int> WS;


   string NPathNoNorm;
   string NPathNorm;
   int Nmin,Nmax,Nstep;

   string KPathNormFileName;
   string KPathMacro;
   string KPathKfold;
   int Kmin, Kmax, Kstep;

   string SVMMacroFoldConfig;
   string SVMPathResult;
   string SVMvideoList;
   string SVMkFoldPath;
   string SVMcsvPath;

    for (int i = 1; i < argc; ++i)
    {
        string Aux(argv[i]);
        if(Aux == "-h"){
            //Escribir como mandar los parametros de cada cosa
            return(1);
        }

        if(Aux == "-RS"){
            //<cantidad de RS> <cantidad de WS> <video_list> <carpeta donde se alojan los ejemplos> <P1> <P2> ... <PN>
            if(argc <= i + 2){
                cout << "Los parametros ingresados no corresponden a esta opcion (-RS) . Intente de nuevo" << endl;
                return(-1);
            }

            int RStest = std::atoi(argv[++i]);
            cout << "RStest: " << RStest << endl;
            int  WStest = std::atoi(argv[++i]);
            cout << "WStest: " << WStest << endl;
            if(argc <= (i + 2 + RStest + WStest) ){
                cout << "Los parametros ingresados  no corresponden a esta opcion (-RS). Intente de nuevo" << endl;
                return(-1);
            }

            RSVideoList = string(argv[++i]);
            cout << RSVideoList << endl;

            RSPathOut = string(argv[++i]);
            cout << RSPathOut << endl;



            for (int j = 0; j < RStest; j++)
            {
                    int AuxInt = std::atoi(argv[++i]);
                    if(AuxInt <=  0){
                        cout << "Los parametros ingreados en los valores no corresponden a esta opcion (ingrese valores mayores que 0)" << endl;
                        return(-1);
                    }
                    cout << AuxInt << endl;
                    RS.push_back(AuxInt);
            }

            for (int j = 0; j < WStest; j++)
            {
                    int AuxInt = std::atoi(argv[++i]);
                    if(AuxInt <=  0){
                        cout << "Los parametros ingreados en los valores no corresponden a esta opcion (ingrese valores mayores que 0)" << endl;
                        return(-1);
                    }
                    cout << AuxInt << endl;
                    WS.push_back(AuxInt);
            }

            if (MicroDescriptorBuilderRun(RSVideoList,RSPathOut,RS,WS) == true){
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
            //<archivo configuracion Normalizado> <path macrodescritptores> <path kfold> <min value> <max value> <step>
            if(argc <= i + 6){
                cout << "Los parametros ingresados no corresponden a esta opcion (-K). Intente de nuevo" << endl;
                return(-1);
            }

            KPathNormFileName = string(argv[++i]);
            KPathMacro = string(argv[++i]);
            KPathKfold = string(argv[++i]);
            Kmin = atoi(argv[++i]);
            Kmax = atoi(argv[++i]);
            Kstep = atoi(argv[++i]);

           if( MacroDescriptorBuilder(KPathNormFileName, KPathMacro, KPathKfold,Kmin, Kmax, Kstep) ){
                cout << "Creacion de macrodescriptres completa" << endl;
           }
           else{
                cout << "hubo un error en la creacion de macrodescriptores :((" << endl;
           }
        }

        if(Aux == "-SVM"){
            //<archivo configuracion de carpetas de macrodescriptores> <path svm results> <video_list> <kfold_path>
             if(argc <= i + 4){
                    cout << "Los parametros ingresados no corresponden a esta opcion (-SVM). Intente de nuevo" << endl;
                    return(-1);
             }

             SVMMacroFoldConfig = string(argv[++i]);
             SVMPathResult = string(argv[++i]);
             SVMvideoList = string(argv[++i]);
             SVMkFoldPath = string(argv[++i]);

             if( SVMkFoldCrossValidation(SVMMacroFoldConfig,SVMPathResult,SVMvideoList,SVMkFoldPath) ){
                cout << "Etapa de obtencion de resultados  de SVM completa" << endl;
            }
            else{
                cout << "Hubo un error en la obtencion de resultados de SVM" << endl;
            }
        }

    }

    return 0;
}