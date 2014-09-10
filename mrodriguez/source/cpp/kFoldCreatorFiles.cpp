#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <chrono>

using namespace std;

class Video
{
	public:
		string VideoPath_;
		int VideoClass_, VideoId_, VideoCFrames_;

	Video(string VideoPath, int VideoClass, int VideoId, int VideoCFrames){
		VideoPath_    = VideoPath;
		VideoClass_   = VideoClass;
		VideoId_ 	  = VideoId;
		VideoCFrames_ = VideoCFrames;
	}

	Video(){}
};

istream& operator>>(istream& is ,  Video &MyVideo)
{
	is >> MyVideo.VideoPath_;
	is >> MyVideo.VideoId_;
	is >> MyVideo.VideoClass_;
	is >> MyVideo.VideoCFrames_;

    return is;
}

ostream& operator<<(ostream& os , Video &MyVideo)
{
	os << endl << MyVideo.VideoPath_    << " ";
	os << MyVideo.VideoId_      << " ";
	os << MyVideo.VideoClass_   << " ";
	os << MyVideo.VideoCFrames_;

    return os;
}


//parametrisar el seed


int main(int argc, char const *argv[])
{
	if(argc < 6){
		cout << "Error en los argumentos" << endl;
		cout << "./Codigo <Lista de entrada> <Ruta de salida> <cantidad de pruebas> <Porcentaje para pruebas> <seed>" << endl;
		return (-1);
	}

	ifstream inList(argv[1]);
	string path(argv[2]); //../foo/kfrold/
	ofstream kFoldConfigFile(path + "kFold_config.txt");


	int CountTest  = std::atoi(argv[3]);
	int Percentaje = std::atoi(argv[4]);
	//int seed = std::atoi(argv[5]);

	map<int,vector<Video>> MapVideos;
	map<int,int> ClassPercentil;

	if (!inList.good() || !kFoldConfigFile.good())
	{
		cout << "El archivo falla" << endl;
		return(-1);
	}


	kFoldConfigFile << CountTest << endl;


	while(!inList.eof()){
		Video newVideo;
		inList >> newVideo;

		MapVideos[newVideo.VideoClass_].push_back(newVideo);
	}


	for (std::map<int,vector<Video>>::iterator MapIt = MapVideos.begin(); MapIt != MapVideos.end(); ++MapIt)
	{
		ClassPercentil[MapIt->first] = (int) std::floor(( Percentaje * MapIt->second.size() ) / 100.0);
	}


	for (int i = 0; i < CountTest; ++i)
	{
		string train = "train_" + std::to_string(i+1) + ".txt";
		string test = "test_" + std::to_string(i+1) + ".txt";

		//cout << "Abro " << train << endl;
		//cout << "Abro " << test << endl;

		kFoldConfigFile << (i+1) << " " << train << " " << test << endl;

		ofstream newTrain(path + train);
		ofstream newTest(path + test);

		if (!newTrain.good() || !newTest.good())
		{
			cout << "Los archivos de salida fallan" << endl;
			return(-1);
		}

		unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);

		for (std::map<int,int>::iterator Map_it = ClassPercentil.begin(); Map_it != ClassPercentil.end(); ++Map_it)
		{
			//cout << "Sacando los de la clase: " << Map_it->first << endl;

		  	std::uniform_int_distribution<int> distribution(0,MapVideos[Map_it->first].size()-1);

  			map<int,bool> RAparition;

		  	int j = 0;
		  	while(j < Map_it->second){
		  		int number = distribution(generator);

		  		//cout << number << endl;

		  		map<int,bool>::iterator it = RAparition.find(number);

				if(it == RAparition.end())
				{
				   j++;
				   RAparition[number] = true;
				   newTest << MapVideos[Map_it->first][number];
				}

		  	}

		  	for (size_t k = 0; k < MapVideos[Map_it->first].size(); ++k)
		  	{
		  		map<int,bool>::iterator it = RAparition.find(k);
		  		if(it == RAparition.end()){
		  			newTrain << MapVideos[Map_it->first][k];
		  		}
		  	}
		}

		newTrain.close();
		newTest.close();
	}

	kFoldConfigFile.close();




	return 0;
}