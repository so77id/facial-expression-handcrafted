#include "../header/kFoldCrossValidation_new.hpp"
#include <iostream>

using namespace std;
using namespace cv;
using namespace utility;

int main(int argc, char const *argv[])
{
    if(argc < 5) {
            cout << "Error en los argumentos" << endl;
            cout << "./programa <Microdescriptores> <video_list> <path kfold> <nClusters>" << endl;
            return (-1);
    }

    string MicroDescriptorsFileName(argv[1]);
    string Video_list(argv[2]);
    string path_kfold(argv[3]);
    size_t nClusters = std::stoi(argv[4]);
    size_t nClass = 6;

//BOVW params
    TermCriteria tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001);
    size_t retries = 1;
    size_t flags = KMEANS_PP_CENTERS;

//SVM params
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF;
    params.gamma = pow(2,-20); //2^-x ---> x elevado
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);

    kFoldCrossValidation myKFCV;
    cout << "Cargando" <<  endl;
    myKFCV.Load(MicroDescriptorsFileName,path_kfold,Video_list);
    myKFCV.RunAll(nClusters, params, nClass, tc , retries , flags );

    return 0;
}