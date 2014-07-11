#include "svm.hpp"
#include <iostream>

using namespace std;


//archivo de macrodescriptores
//archivo original de los videos
//ruta al archivo de configuracion del kfold


int main(int argc, char const *argv[])
{
	if(argc < 4) {
			cout << "Error en los argumentos" << endl;
			cout << "./programa <Macrodescriptores> <video_list> <path kfold>" << endl;
			return (-1);
	}

	string macrodescriptores(argv[1]);
	string video_list(argv[2]);
	string path_kfold(argv[3]);

	CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF;
    params.gamma = 20;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);


	kFoldCrossValidation kFold(macrodescriptores,video_list,path_kfold, params);

	cout << "Voy a cargar los descriptores" << endl;
	kFold.loadDescriptors();
	cout << "Comienzo con la kfold" << endl;
	kFold.runKfoldCrossValidation();

	cout << "Accuracy: " << kFold.GetAccuracy();

	return 0;
}