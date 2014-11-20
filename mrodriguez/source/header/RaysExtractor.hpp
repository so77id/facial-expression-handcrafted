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
        bool debug;
        double MSE(const int, const int, const int, const int, const int, Mat&, Mat&);
        //pixel Match(const int, const int, const int, const int, const int, const int, Mat&, Mat&);
        pixel Match(const pixel, const int, const int, cv::Mat&, cv::Mat&);
    public:
        RaysExtractor(const bool);
        MapRaysFlux Extract(VideoCapture &,const int, const int, const bool);
        RayFlux Normalize(const RayFlux rays,const int SizeNorm);
};

RaysExtractor::RaysExtractor(const bool debug = false){
    this->debug = debug;
}


double RaysExtractor::MSE(const int irs, const int jrs, const int iws, const int jws, const int RSsize, Mat &f0, Mat &f1){
        double sum = 0.0;

        //if(debug)
        //    cout << "\nLa region de soporte de WS\n";

        for(int i = 0; i < RSsize; i++)
        {
            for(int j = 0; j < RSsize; j++)
            {
                //if(debug)
                //    std::cout << int(f1.at<uchar>( (i+iws), (j+jws) )) << " ";
                sum +=  std::pow( f1.at<uchar>( (i+iws) , (j+jws) ) - f0.at<uchar>( (i+irs), (j+jrs) ) , 2);
            }

            //if(debug)
            //    std::cout << std::endl;
        }

        return ( std::sqrt(sum) );
}


pixel RaysExtractor::Match(const pixel Actual, const int SRsize, const int WSsize, cv::Mat &frame0, cv::Mat &frame1)
{
    int isr,jsr,iws,jws, iwsMin, jwsMin, iwsMax, jwsMax;
    double min, tmp;
    pixel PixelOp;
    vector<pixel> minList;

    min            = std::numeric_limits<double>::max();
    isr              = Actual.first - ((SRsize-1)/2);
    jsr              = Actual.second - ((SRsize-1)/2);
    iwsMin       =  Actual.first - ((WSsize-1) / 2);
    jwsMin       = Actual.second - ((WSsize-1) / 2);
    iwsMax      = Actual.first + ((WSsize-1) / 2) - ((SRsize -1)/2);
    jwsMax      = Actual.second + ((WSsize-1) / 2) - ((SRsize -1)/2) ;

/* Buscar reglas para optimzar el proceso de busqueda sacando los if, para tener indices inteligentes al recorrer el WS
    Calculos en el cuaderno.
    iwsMin = iwsMin >= 0 ? iwsMin : 0;
    jwsMin = jwsMin >= 0 ? jwsMin : 0;
    iwsMax = iwsMax < frame1.rows ? iwsMax :
*/

    /*  debugin  */

    /*if(debug)
    {
        std::cout << "Support Region" << std::endl;
        for(int i = isr; i < isr + SRsize; i++)
        {
            for(int j = jsr; j < jsr + SRsize; j++)
            {
                std::cout << int(frame0.at<uchar>(i,j)) << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "--------------------------------------------------\n\n";

        std::cout << "Window Search" << std::endl;
        for(int i = iwsMin; i < iwsMin + WSsize; i++)
        {
            for(int j = jwsMin; j < jwsMin + WSsize; j++)
            {
                if(i < 0 || j < 0 || i >= frame1.rows || j >= frame1.cols) continue;
                std::cout << int(frame1.at<uchar>(i,j)) << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "--------------------------------------------------\n\n";
    }
/* fin debugin*/


    for (iws = iwsMin; iws < iwsMax; ++iws)
    {
        for (jws = jwsMin; jws < jwsMax; ++jws)
        {
            if(iws < 0 || jws < 0 || (iws + SRsize) > frame1.rows || (jws + SRsize) > frame1.cols) {
                //cout << "continuo con: (" << iws << "," << jws << ")\n";
                continue;
            }

           /* if(debug)
                std::cout << "\n\npixel central: (" << iws + ((SRsize-1)/2) << "," << jws + ((SRsize-1)/2) << ")\n";

            if(debug)
                std::cout << "LLamando al MSE\n";
        */
            tmp = MSE(isr, jsr, iws, jws, SRsize, frame0, frame1);

           // if(debug)
            //    std::cout << "El MSE es:" << tmp << std::endl;

            if(tmp < min){
                min = tmp;
                minList.clear();

                minList.push_back(std::make_pair( iws + ((SRsize-1)/2), jws + ((SRsize-1)/2) ) );
            }
            else if(tmp == min){
                minList.push_back(std::make_pair( iws + ((SRsize-1)/2), jws + ((SRsize-1)/2) ) );
            }

        }
    }


    min = std::numeric_limits<double>::max();
    for (std::vector<pixel>::iterator it = minList.begin(); it != minList.end(); ++it)
    {
        double i = ((isr + (SRsize-1)/2) - it->first) * ((isr + (SRsize-1)/2) - it->first);
        double j = ((jsr + (SRsize-1)/2) - it->second) * ((jsr + (SRsize-1)/2) - it->second);

        double distance = std::sqrt(i+j);

        if(distance < min) {
            PixelOp = std::make_pair(it->first, it->second);
            min = distance;
        }
    }


   /* if(debug)
        std::cout << "El Pixel escogido es: " << PixelOp.first << " " << PixelOp.second << std::endl;
    */
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

    Mat frame0;
    Mat frame1;

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

                /*if(constDebug && p1.first == Map_it->first.first && p1.second == Map_it->first.second)
                    debug = true;
                else
                    debug = false;
                */
                /*
                if(debug)
                    cout << "Pixel por revisar: (" << ActualPixel.first << "," << ActualPixel.second << ")\n";
    */


                PixelOp = std::move( Match (ActualPixel , SRsize, WSsize, frame0, frame1 )) ;

      /*          if(debug)
                    std::cin.get();*/
                //while(waitKey(0) != 13);
                //PixelOp = std::move( Match ( i, j, i-((WSsize-1)/2) +((SRsize-1)/2), j - ((WSsize-1)/2) +((SRsize-1)/2), SRsize, WSsize, frame0, frame1 )) ;
                Map_it->second = PixelOp;
                //RaySupport newRaySupport = std::make_pair( float(ActualPixel.first - OptimalPixel.first) / float(nFrames), float(ActualPixel.second - OptimalPixel.second) / float(nFrames) );
                RaySupport newRaySupport;

                if(visual)
                    newRaySupport = std::make_pair( float(PixelOp.first), float(PixelOp.second)  );
                else
                     newRaySupport = std::make_pair( float(ActualPixel.first - PixelOp.first), float(ActualPixel.second - PixelOp.second)  );

                RaysMap[Map_it->first].push_back(newRaySupport);
        }
        frame1.copyTo(frame0);
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


/*
pixel RaysExtractor::Match(const int irs, const int jrs, const int iws, const int jws, const int SRsize, const int WSsize, Mat &f0, Mat &f1){
    double min = std::numeric_limits<double>::max();
    pixel PixelOp; //pixel optimo de movimiento

    vector<pixel> minList;
/
    cout << irs << "," << jrs << endl;
    cout << iws << "," << iws << endl;

    std::cout << "Support Region" << std::endl;
    for(int i = irs; i < irs + SRsize; i++)
    {
        for(int j = jrs; j < jrs + SRsize; j++)
        {
            std::cout << int(f0.at<uchar>(i,j)) << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "--------------------------------------------------\n\n";

    std::cout << "Window Search" << std::endl;
    for(int i = iws; i < iws + WSsize; i++)
    {
        for(int j = jws; j < jws + WSsize; j++)
        {
            std::cout << int(f1.at<uchar>(i,j)) << " ";
        }
        std::cout << std::endl;
    }

/

     for(int i = iws; i < iws + WSsize; i++){

        for(int j = jws; j < jws + WSsize; j++){

            if (i < 0 || j <0 || i >= f1.rows || j >= f1.cols) continue;

            //std::cout << "\n\npixel: (" << i << "," << j << ")\n";
            //std::cout << "LLamando al MSE\n";

            double tmp = MSE(irs, jrs, i, j, SRsize, f0, f1);

            //std::cout << "El MSE es:" << tmp << std::endl;

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

    //std::cout << "El Pixel escogido es: " << PixelOp.first << " " << PixelOp.second << std::endl;

    //while(waitKey(0) != 13);

    return(PixelOp);
}
*/