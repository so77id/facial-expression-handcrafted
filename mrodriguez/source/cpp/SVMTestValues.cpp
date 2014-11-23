#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "../header/kFoldCrossValidation_new.hpp"


using namespace std;


//GMin GMax GStep
//CMin CMax CStep


int main(int argc, char const *argv[])
{
    if(argc < 8) {
        cout << "Error en los argumentos" << endl;
        cout << "./programa <MacrodescriptoresConfigFileName> <video_list> <path kfold> <RBFFile> <LinealFile> <PolyFile> <SigmoidFile>" << endl;
        return (-1);
    }

    string MacroConfig(argv[1]);
    string video_list(argv[2]);
    string kfoldPath(argv[3]);
    string RBFfile(argv[4]);
    string LinealFile(argv[5]);
    string PolyFile(argv[6]);
    string SIgmoidFile(argv[7]);


    int GammaMin = -30;
    int GammaMax = 5;
    int CMin = -10;
    int CMax = 15;
    int DegreeMin = 1;
    int DegreeMax = 10;

    kFoldCrossValidation myKFCV;
    myKFCV.LoadForSVM(MacroConfig,kfoldPath, video_list);

    //SVM params
   /* CvSVMParams Params;
    Params.svm_type    = CvSVM::C_SVC;
    Params.kernel_type = CvSVM::RBF;
    Params.gamma = pow(2,-20); //2^-x ---> x elevado
    Params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);
    */
//RBF
    //Gamma 2^-20 -> 2^4
    //C 2^-5 -> 2^15
    CvSVMParams RBFParams;
    RBFParams.svm_type  = CvSVM::C_SVC;
    RBFParams.kernel_type = CvSVM::RBF;
    RBFParams.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);
    ofstream RBFout(RBFfile);
    if(!RBFout.good())
    {
        cout << "Error al abrir el archivo de RBF" << endl;
        return(-1);
    }


    for (int i = GammaMin; i <= GammaMax; ++i)
    {
        for (int j = CMin; j <= CMax; ++j)
        {
            RBFParams.gamma = pow(2,i);
            RBFParams.C = pow(2,j);
            myKFCV.RunSVM(RBFParams,6);
            RBFout << RBFParams.gamma << "\t" << RBFParams.C << "\t" << myKFCV.GetAccuracy() << endl;
            cout << "Gamma: " << RBFParams.gamma << " C: " << RBFParams.C  << " Accuracy: " << myKFCV.GetAccuracy() << endl;
        }
    }

    RBFout.close();

//Lineal
    //C 2^-5 -> 2^15
    CvSVMParams LINEALParams;
    LINEALParams.svm_type  = CvSVM::C_SVC;
    LINEALParams.kernel_type = CvSVM::LINEAR;
    LINEALParams.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);
    ofstream LINEALout(LinealFile);
    if(!LINEALout.good())
    {
        cout << "Error al abrir el archivo de Linear" << endl;
        return(-1);
    }

        for (int j = CMin; j <= CMax; ++j)
        {
            LINEALParams.C = pow(2,j);
            myKFCV.RunSVM(LINEALParams,6);
            LINEALout << LINEALParams.C << "\t" << myKFCV.GetAccuracy() << endl;
            cout << "C: " << LINEALParams.C  << " Accuracy: " << myKFCV.GetAccuracy() << endl;
        }

    LINEALout.close();

//SIgmoid
    //Gamma 2^-20 -> 2^4
    //coef 0
    //C 2^-5 -> 2^15

    CvSVMParams SigmoidParams;
    SigmoidParams.svm_type  = CvSVM::C_SVC;
    SigmoidParams.kernel_type = CvSVM::SIGMOID;
    SigmoidParams.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);
    ofstream SIGMOIDout(SIgmoidFile);
    if(!SIGMOIDout.good())
    {
        cout << "Error al abrir el archivo de SIGMOID" << endl;
        return(-1);
    }


    for (int i = GammaMin; i <= GammaMax; ++i)
    {
        for (int j = CMin; j <= CMax; ++j)
        {
            SigmoidParams.gamma = pow(2,i);
            SigmoidParams.C = pow(2,j);
            myKFCV.RunSVM(SigmoidParams,6);
            SIGMOIDout << SigmoidParams.gamma << "\t" << SigmoidParams.C << "\t" << myKFCV.GetAccuracy() << endl;
            cout << "Gamma: " << SigmoidParams.gamma << " C: " << SigmoidParams.C  << " Accuracy: " << myKFCV.GetAccuracy() << endl;
        }
    }

    SIGMOIDout.close();



//Poly
    //degree 1 -> 10
    //coef 0
    //Gamma 2^-20 -> 2^4
    //C 2^-5 -> 2^15

    CvSVMParams PolyParams;
    PolyParams.svm_type  = CvSVM::C_SVC;
    PolyParams.kernel_type = CvSVM::POLY;
    PolyParams.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);
    ofstream POLYout(PolyFile);
    if(!POLYout.good())
    {
        cout << "Error al abrir el archivo de POLY" << endl;
        return(-1);
    }


    for (int i = GammaMin; i <= GammaMax; ++i)
    {
        for (int j = CMin; j <= CMax; ++j)
        {
            for (int z = DegreeMin; z <= DegreeMax; ++z)
            {
                PolyParams.degree = z;
                PolyParams.gamma = pow(2,i);
                PolyParams.C = pow(2,j);
                myKFCV.RunSVM(PolyParams,6);
                POLYout << PolyParams.gamma << "\t" << PolyParams.degree << "\t"<< PolyParams.C << "\t" << myKFCV.GetAccuracy() << endl;
                cout << "Gamma: " << PolyParams.gamma << " Degree: " << PolyParams.degree << " C: " << PolyParams.C  << " Accuracy: " << myKFCV.GetAccuracy() << endl;
            }
        }
    }

    POLYout.close();

    return 0;
}