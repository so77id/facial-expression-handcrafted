#include "../header/kFoldCrossValidation_new.hpp"
#include <iostream>

using namespace std;
using namespace cv;
using namespace utility;

int main(int argc, char const *argv[])
{
    if(argc < 6) {
            cout << "Error en los argumentos" << endl;
            cout << "./programa <Microdescriptores> <video_list> <path kfold> <nClusters> <path_macro>" << endl;
            return (-1);
    }

    string MicroDescriptorsFileName(argv[1]);
    string Video_list(argv[2]);
    string path_kfold(argv[3]);
    size_t nClusters = std::stoi(argv[4]);
    string PathMacro(argv[5]);


//BOVW params
    TermCriteria tc = TermCriteria(CV_TERMCRIT_ITER,100,0.001);
    size_t retries = 1;
    size_t flags = KMEANS_PP_CENTERS;

    kFoldCrossValidation myKFCV;
    myKFCV.LoadForBuildMacros(MicroDescriptorsFileName,path_kfold);
    myKFCV.RunMacroDescriptorBuilderkFold(PathMacro,nClusters, tc , retries , flags );

    return 0;
}