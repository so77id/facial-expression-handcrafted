#include "opencv2/opencv.hpp"
#include <vector>
#include <utility>
#include <limits>
#include <cmath>
#include "utility.hpp"

using namespace cv;
using namespace std;
using namespace utility;

/*
    typedef pair<int,int> pixel; // first -> es la coordenada X,  second -> es la coordenada Y
    typedef pair<pair<int,int>,int> voxel; // first -> pixel (con first -> X, second -> Y) ; second -> coordenada T
    typedef pair<float,float> RaySupport; //Rayo de soporte, unidad minima de un rayo de flujo (ray flux)
    typedef vector<RaySupport> RayFlux; //Rayo de flujo
    typedef vector<RayFlux> ListRaysFlux; // vector de rayos de flujo
    typedef map<pixel,RayFlux> MapRaysFlux; //Mapa para saber que rayo le pertence a que pixel
*/

class RaysExtractor
{
    private:
        double MSE(const int, const int, const int, const int, const int, Mat&, Mat&);
        pixel Match(const int, const int, const int, const int, const int, const int, Mat&, Mat&);
    public:
        MapRaysFlux Extract(VideoCapture &,const int, const int, const bool );
        RayFlux Normalize(const RayFlux rays,const int SizeNorm);
};


double RaysExtractor::MSE(const int iws, const int jws, const int irs, const int jrs, const int RSsize, Mat &f0, Mat &f1){
        double sum = 0.0;

        for(int i = 0; i < RSsize; i++)
        {
            for(int j = 0; j < RSsize; j++)
            {
                sum +=  std::pow( f1.at<uchar>( (i+iws) , (j+jws) ) - f0.at<uchar>( (i+irs), (j+jrs) ) , 2);
            }
        }

        return ( std::sqrt(sum) );
}


pixel RaysExtractor::Match(const int irs, const int jrs, const int iws, const int jws, const int SRsize, const int WSsize, Mat &f0, Mat &f1){
    double min = std::numeric_limits<double>::max();
    pixel PixelOp; //pixel optimo de movimiento

    vector<pixel> minList;

    for(int i = iws; i < iws + WSsize; i++){

        for(int j = jws; j < jws + WSsize; j++){

            if (i < 0 || j <0 || i > f1.rows || j > f1.cols) continue;

            double tmp = MSE(i, j, irs, jrs, SRsize, f0, f1);

            if(tmp < min){
                min = tmp;
                minList.clear();

                minList.push_back(std::make_pair(i,j));
            }
            else if(tmp == min){
                minList.push_back(std::make_pair(i,j));
            }
        }
    }


    min = std::numeric_limits<double>::max();
    for (std::vector<pixel>::iterator it = minList.begin(); it != minList.end(); ++it)
    {
        double i = ((irs + (SRsize-1)/2) - it->first) * ((irs + (SRsize-1)/2) - it->first);
        double j = ((jrs + (SRsize-1)/2) - it->second) * ((jrs + (SRsize-1)/2) - it->second);

        double distance = std::sqrt(i+j);

        if(distance < min) {
            PixelOp = std::make_pair(it->first, it->second);
            min = distance;
        }

    }

    return(PixelOp);
}


//SRsize  tamaño de la region de soporte
MapRaysFlux RaysExtractor::Extract(VideoCapture &video, const int SRsize, const int WSsize, const bool visual = false)
{
    if(!video.isOpened()){
        cout << "Video file error" << endl;
        MapRaysFlux RaysRoi;
        return(RaysRoi);
    }

    //cuento la cantidad de frames del video
    /*int nFrames = video.get(CV_CAP_PROP_FRAME_COUNT) - 1;

    if(nFrames < 2){
        cout << "La cantidad de frames de este video es menor que 2" << endl;
        MapRaysFlux RaysRoi;
        return (RaysRoi);
    }
    */

    Mat frame0;
    Mat frame1;

    //video >> frame0;
    video.read(frame0);

    map<pixel,pixel> MovimentMap; //Mapa que permite ver a cual pixel se movio en el siguiente tiempo (t+1)
    //En el caso del primer pixel parte con un movimiento a si mismo

    for(int i = (SRsize-1)/2; i < frame0.rows - (SRsize-1)/2; i++ )
    {
        for(int j = (SRsize-1)/2; j < frame0.cols -(SRsize-1)/2; j++)
        {
            pixel ActualPixel = make_pair(i,j);
            MovimentMap[ActualPixel] = ActualPixel;
        }
    }

    MapRaysFlux  RaysMap;
    pixel PixelOp;

    while(video.read(frame1))
    {
        for (std::map<pixel,pixel>::iterator Map_it = MovimentMap.begin(); Map_it != MovimentMap.end(); ++Map_it)
        {
                pixel ActualPixel = Map_it->second;
                int i = ActualPixel.first;
                int j = ActualPixel.second;

                PixelOp = std::move( Match ( i, j, i-((WSsize-1)/2) +((SRsize-1)/2), j - ((WSsize-1)/2) +((SRsize-1)/2), SRsize, WSsize, frame0, frame1 )) ;
                Map_it->second = PixelOp;
                //RaySupport newRaySupport = std::make_pair( float(ActualPixel.first - OptimalPixel.first) / float(nFrames), float(ActualPixel.second - OptimalPixel.second) / float(nFrames) );
                RaySupport newRaySupport;

                if(visual)
                    newRaySupport = std::make_pair( float(PixelOp.first), float(PixelOp.second)  );
                else
                     newRaySupport = std::make_pair( float(ActualPixel.first - PixelOp.first), float(ActualPixel.second - PixelOp.second)  );

                RaysMap[Map_it->first].push_back(newRaySupport);
        }

        frame0 = frame1;
    }

    return(RaysMap);
}

RayFlux RaysExtractor::Normalize(const RayFlux rays,const int SizeNorm){
    float rate = float(rays.size()) / float(SizeNorm) ;
    RayFlux NormalizeRays;

    float first,second,RemanentRate;
    float RemanentActual = 1.0;
    for (int i = 0,j = 0; i < SizeNorm; ++i)
    {
        first        = 0.0;
        second   = 0.0;
        RemanentRate  = rate;

        while(RemanentRate > 0.0){
            if(RemanentRate >= RemanentActual){
                first                  += RemanentActual * float(rays[j].first);
                second             += RemanentActual * float(rays[j].second);
                RemanentRate  -= RemanentActual;
                RemanentActual = 1.0;
                j++;
            }
            else{
                first                  += RemanentRate * float(rays[j].first);
                second              += RemanentRate * float(rays[j].second);
                RemanentActual -= RemanentRate;
                RemanentRate    = 0.0;
            }
        }

        NormalizeRays.push_back(make_pair(first,second));
    }

    return (NormalizeRays);
}