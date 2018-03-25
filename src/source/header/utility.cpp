#include "utility.hpp"
#include "lbp.cpp"
#include "ldn.cpp"
#include "mask.cpp"

using namespace cv;

    vector<string> utility::split(string str,string sep){
        char* cstr = const_cast<char*>(str.c_str());
        char* current;
        vector<string> arr;
        current = strtok(cstr,sep.c_str());
        while(current != NULL){
            arr.push_back(current);
            current=strtok(NULL,sep.c_str());
        }
        return arr;
    }

    pair<double,double> utility::mean( RayFlux & rayo){

        double sumx, sumy;
        sumx = sumy = 0.0;

        for (RayFlux::iterator it = rayo.begin(); it != rayo.end(); ++it)
        {
            sumx += float(it->first);
            sumy += float(it->second);
        }

        sumx /= (rayo.size()*1.0);
        sumy /= (rayo.size()*1.0);

        return(std::make_pair(sumx, sumy));
    }


    pair<double,double> utility::StandarDeviation( RayFlux & rayo){

        pair<double,double> sum = std::move(mean(rayo));
        double meanx = sum.first, meany = sum.second;
        sum.first = sum.second = 0.0;

        for (RayFlux::iterator it = rayo.begin(); it != rayo.end(); ++it)
        {
            //cout << it->first << " " << meanx  << " " << (it->first - meanx) << " " << (it->first - meanx) << endl;
            sum.first     += (it->first - meanx) * ( it->first - meanx);
            sum.second += (it->second - meany) * ( it->second - meany);
        }

        /*
        cout << sum.first << " " << rayo.size() << " " << sum.first / rayo.size() << endl;
        cin.get();
        */

//      sum.first /= (rayo.size() * 1.0);
//      sum.second /= (rayo.size() * 1.0);

        sum.first = std::sqrt(  sum.first  / (rayo.size() *1.0) );
        sum.second = std::sqrt( sum.second / (rayo.size()*1.0) );

        return(sum);
    }



    float utility::TransformLabels(int i){
        float dato = 0.0;

        switch (i){
            case 1:
                dato = -1.0;
                break;
            case 2:
                dato = -0.6;
                break;
            case 3:
                dato = -0.2;
                break;
            case 4:
                dato = 0.2;
                break;
            case 5:
                dato = 0.6;
                break;
            case 6:
                dato = 1.0;
                break;
        }
        return dato;
    }
