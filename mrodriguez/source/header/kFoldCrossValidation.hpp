#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "utility.hpp"
#include <cstdio>

using namespace std;
using namespace cv;
using namespace utility;


    //typedef vector<int> Macrodescriptor; // Macrodescriptor
    //typedef vector<Macrodescriptor> ListMacrodescriptors; //Vector de macrodescriptores;
    //typedef vector<int> ListLabels; //Lista de labels de los macrodescriptores
    //typedef ListMacrodescriptors::iterator ListMacrodescriptorIter;
    //typedef  ListLabels::iterator ListLabelIter;
    //typedef vector<int> Set; //vector con los indices de los videos incluidos en un cierto set
    //typedef pair<Set,Set> DataSet; //first son los datos de entrenamiento y second los datos de pruebas
    //typedef map<int,DataSet> MapDataSet; // key es el numero de la prueba del k-fold y el value es el dataset de la prueba
    //typedef MapDataSet::iterator MapDataSetIter; //Iterador del map de datasets;



class kFoldCrossValidation
{
    private:
        ifstream kFoldConfigFile_; //Archivo con los datos con las rutas a cada una de las instancias de prueba
        ifstream MacroDescriptorFile_; //Archivo con los macrodescriptores
        ifstream LabelsFile_;
        string kFoldPath_;
        int nClusters_; // Cantidad de clusters de la prueba
        int kFolds_; //Cantidad de pruebas

        CvSVMParams Params_; // Parametros del SVM

        Mat Data_, LabelData_;

        vector<double> ListAccuracy_;

        MapDataSet kFoldInstance;


        double GetAccuracy();
    public:
        kFoldCrossValidation(string, string, string, CvSVMParams);
        ~kFoldCrossValidation();
        bool loadDescriptors();
        void buildConfusionMatrix();
        double runKfoldCrossValidation();
};


kFoldCrossValidation::kFoldCrossValidation(string MacroDescriptorFile, string kFoldPath, string LabelsFile, CvSVMParams Params){
    kFoldPath_ = kFoldPath;
    kFoldConfigFile_.open(kFoldPath_ + "kFold_config.txt");
    MacroDescriptorFile_.open(MacroDescriptorFile);
    LabelsFile_.open(LabelsFile);
    Params_ = Params;

}


kFoldCrossValidation::~kFoldCrossValidation(){
    //delete(Data_);
    //delete(LabelData_);
    kFoldConfigFile_.close();
    MacroDescriptorFile_.close();
    LabelsFile_.close();
}

bool kFoldCrossValidation::loadDescriptors()
{
        if( !kFoldConfigFile_.good() || !MacroDescriptorFile_.good() || !LabelsFile_.good()){
                cout << "No pude abrir los archivos" << endl;
                return(false);
        }

//=====================================================================
// Cargando los macrodescriptores

        int id, feature;
        ListMacrodescriptors TotalMacrodescriptors;

        MacroDescriptorFile_ >> nClusters_;

        while(! MacroDescriptorFile_.eof()){

            MacroDescriptorFile_ >> id;

            Macrodescriptor newMacrodescriptor;

            for (int i = 0; i < nClusters_; ++i){

                MacroDescriptorFile_ >> feature;
                newMacrodescriptor.push_back(feature);
            }

            TotalMacrodescriptors.push_back(std::move(newMacrodescriptor));
        }

//=====================================================================
// Cargando los labels

        int index, class_, frames;
        string buffer;

        ListLabels TotalLabels;

        while(!LabelsFile_.eof()){
            LabelsFile_ >> buffer >> index >> class_ >> frames;
            TotalLabels.push_back(class_);
        }

//=====================================================================
// Se verifica que exista la misma cantidad de labels como de microdescriptores

        if(TotalLabels.size() != TotalMacrodescriptors.size()){
            cout << "La cantidad de descriptores no concuerda con la cantidad de etiquetas" << endl;
            return (false);
        }

//=====================================================================
//Cargando los datos del k-fold

        string train, test;
        string path;
        int video_id;

        kFoldConfigFile_ >> kFolds_;

        //cout << kFolds_ << endl;

        for (int i = 0; i < kFolds_; ++i)
        {
            kFoldConfigFile_ >> id >> train >> test;
            //cout << id << " " << train << " " << test << endl;

            ifstream trainFile( kFoldPath_ + train);
            ifstream testFile( kFoldPath_ + test);

            if(!trainFile.good() || !testFile.good()){
                cout << "error en la lectura de un archivo del k-fold" << endl;
                return (false);
            }

            //cout << "TrainFile: " << endl;
            while(!trainFile.eof()){
                trainFile >> path >> video_id >> class_ >> frames;
                //cout << "\t" << video_id << endl;
                kFoldInstance[i].first.push_back(video_id - 1);
            }

            //cout << "TestFile: " << endl;
            while(!testFile.eof()){
                testFile >> path >> video_id >> class_ >> frames;
                //cout << "\t" << video_id << endl;
                kFoldInstance[i].second.push_back(video_id - 1);
            }

            if( ( kFoldInstance[i].second.size() + kFoldInstance[i].first.size() ) != TotalMacrodescriptors.size() ){
                cout << "El k-fold " << i+1<< "tiene erroes ya que la suma de train + test != size microdescriptores" << endl;
                return(false);
            }

        }

/* codigo para probar que los k-fold cargaron bien
        for (MapDataSetIter Map_it = kFoldInstance.begin(); Map_it != kFoldInstance.end(); ++Map_it)
        {
            cout << "DataSet :" << Map_it->first << endl;

            cout << "Train" << endl;
            for (Set::iterator Set_it = Map_it->second.first.begin(); Set_it != Map_it->second.first.end(); ++Set_it)
            {
                cout << *Set_it << " ";
            }
            cout << endl << "Test" << endl;

            for (Set::iterator Set_it = Map_it->second.second.begin(); Set_it != Map_it->second.second.end(); ++Set_it)
            {
                cout << *Set_it << " ";
            }
            cout << endl;
        }
*/
//=====================================================================
// Se transpasan a Mat los datos de los descriptores y los labels

        Mat AuxData_( TotalMacrodescriptors.size(),nClusters_,CV_32F);
        Mat AuxLabelData_( TotalLabels.size(),1,CV_32F);

        AuxData_.copyTo(Data_);
        AuxLabelData_.copyTo(LabelData_);

        int row = 0,col;
        ListLabelIter Label_it = TotalLabels.begin();
        for (ListMacrodescriptorIter Macro_it = TotalMacrodescriptors.begin();
              Macro_it != TotalMacrodescriptors.end() && Label_it != TotalLabels.end();
              ++Macro_it)
        {
                col = 0;
                LabelData_.at<float>(row,col) = *Label_it;
                for (Macrodescriptor::iterator feature_it = Macro_it->begin(); feature_it != Macro_it->end(); ++feature_it)
                {
                    Data_.at<float>(row,col) = *feature_it;
                    col++;
                }
                row++;
                 ++Label_it;
        }

        //cout << LabelData_ << endl << endl;
        //cout << Data_ << endl << endl;


        return(true);

}


double kFoldCrossValidation::GetAccuracy(){

    double accuracy = 0.0;
    for (vector<double>::iterator it = ListAccuracy_.begin(); it != ListAccuracy_.end(); ++it)
    {
        accuracy += *it;
    }

    return( accuracy / (1.0 * ListAccuracy_.size()) );
}

double kFoldCrossValidation::runKfoldCrossValidation(){

    int row = 0;

    for (int k = 0; k < kFolds_; ++k)
    {
        Mat TrainSet(kFoldInstance[k].first.size(),nClusters_,CV_32F);
        Mat TrainLabelSet(kFoldInstance[k].first.size(),1,CV_32F);
        Mat TestSet(kFoldInstance[k].second.size(),nClusters_,CV_32F);
        Mat TestLabelSet(kFoldInstance[k].second.size(),1,CV_32F);
        Mat PredictLabelSet(kFoldInstance[k].second.size(),1,CV_32F);

        row = 0;
        for (Set::iterator Set_it = kFoldInstance[k].first.begin(); Set_it != kFoldInstance[k].first.end(); ++Set_it)
        {
            Data_.row(*Set_it).copyTo(TrainSet.row(row));
            LabelData_.row(*Set_it).copyTo(TrainLabelSet.row(row));
            row++;
        }

        row = 0;
        for (Set::iterator Set_it = kFoldInstance[k].second.begin(); Set_it != kFoldInstance[k].second.end(); ++Set_it)
        {
            Data_.row(*Set_it).copyTo(TestSet.row(row));
            LabelData_.row(*Set_it).copyTo(TestLabelSet.row(row));
            row++;
        }

        CvSVM SVM_;
        SVM_.train(TrainSet, TrainLabelSet, Mat(), Mat(), Params_);

        SVM_.predict(TestSet,PredictLabelSet);

        PredictLabelSet = (PredictLabelSet == TestLabelSet) / 255;

        //Revisar el uso de la funcion SUM de opencv
        double accuracy = mean(PredictLabelSet.col(0))[0];
        ListAccuracy_.push_back(accuracy);
        //cout << accuracy << endl;
    }

    return(GetAccuracy());

}