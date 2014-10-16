#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "../header/kFoldCrossValidation.hpp"


using namespace std;


//GMin GMax GStep
//CMin CMax CStep


int main(int argc, char const *argv[])
{
    if(argc < 11){
        cout << "error, intente: ./programa <Gmin> <GMax> <GStep> <CMin> <CMax> <CStep> <MicroFileName> <Video_list>  <KFoldPath> <OutFileName>" << endl;
        return (-1);
    }

    int GMin, GMax, GStep, CMin, CMax, CStep;
    float Accuracy;
    string MacroFileName, VideoListFileName, KFoldPath, OutFileName;

    GMin = std::atoi(argv[1]);
    GMax = std::atoi(argv[2]);
    GStep = std::atoi(argv[3]);
    CMin = std::atoi(argv[4]);
    CMax = std::atoi(argv[5]);
    CStep = std::atoi(argv[6]);
    MacroFileName = string(argv[7]);
    VideoListFileName = string(argv[8]);
    KFoldPath = string(argv[9]);
    OutFileName = string(argv[10]);


    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);


    ofstream OutFile(OutFileName);

    if(!OutFile.good()){
        cout << "No se puede abrir el archivo de salida" << endl;
        return(-1);
    }

    for (int G_it = GMin; G_it <= GMax; G_it += GStep)
    {
        for (int C_it = CMin; C_it <= CMax; C_it += CStep)
        {
            params.gamma = pow(2,G_it); //2^-x ---> x elevado
            params.C = pow(10,C_it);

            cout << "Iniciando SVM con gamma_it: " << G_it << " C_it:" << C_it << endl;

            kFoldCrossValidation kFold(MacroFileName, KFoldPath, VideoListFileName, params);

            if( false == kFold.loadDescriptors()){
                cout << "Error al cargar los descriptores" <<  endl;
                return(-1);
            }

            Accuracy =  kFold.runKfoldCrossValidation();

            OutFile << Accuracy << " " << params.gamma << " " << params.C << endl;
        }
    }

    return 0;
}