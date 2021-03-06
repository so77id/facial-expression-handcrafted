#include "../header/kFoldCrossValidation.hpp"
#include <iostream>

using namespace std;


//archivo de macrodescriptores
//archivo original de los videos
//ruta al archivo de configuracion del kfold


int main(int argc, char const *argv[])
{
    if(argc < 5) {
            cout << "Error en los argumentos" << endl;
            cout << "./programa <Macrodescriptores> <video_list> <path kfold> <CSV File>" << endl;
            return (-1);
    }

    string macrodescriptores(argv[1]);
    string video_list(argv[2]);
    string path_kfold(argv[3]);
    string CSV_File(argv[4]);

    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF;
    params.gamma = pow(2,-20); //2^-x ---> x elevado
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);

    kFoldCrossValidation kFold(macrodescriptores, path_kfold, video_list, params);

    cout << "Voy a cargar los descriptores" << endl;
    if( false == kFold.loadDescriptors()){
        cout << "Error al cargar los descriptores" << endl;
        return(-2);
    }

    cout << "Comienzo con la kfold" << endl;
    cout << "Accuracy: " <<  kFold.runKfoldCrossValidation() << endl;

    ofstream OutCSV(CSV_File);
    if(OutCSV.good())  kFold.GetConfusionMatrixCSV(OutCSV);
    OutCSV.close();


    return 0;
}