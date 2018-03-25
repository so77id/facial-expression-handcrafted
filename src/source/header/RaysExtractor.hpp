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
    protected:
        bool debug;
        Mat gauss;
        double MSE(const int, const int, const int, const int, const int, Mat&, Mat&);
        Mat Gauss(const int);
        //pixel Match(const int, const int, const int, const int, const int, const int, Mat&, Mat&);
        pixel Match(const pixel, const int, const int, cv::Mat&, cv::Mat&);
    public:
        RaysExtractor(const bool);
        virtual MapRaysFlux Extract(VideoCapture &,const int, const int, const bool);
        RayFlux Normalize(const RayFlux rays,const int SizeNorm);
};

RaysExtractor::RaysExtractor(const bool debug = false){
    this->debug = debug;
}

Mat RaysExtractor::Gauss(const int RSsize)
{
    gauss  = Mat::zeros(RSsize, RSsize, CV_32FC1);
    float sigmaX = 0.0;
    float sigmaY = 0.0;

    Mat kernelX = getGaussianKernel(RSsize, sigmaX);
    Mat kernelY = getGaussianKernel(RSsize, sigmaY);
    gauss = kernelX * kernelY.t();

    return(gauss);
}


double RaysExtractor::MSE(const int irs, const int jrs, const int iws, const int jws, const int RSsize, Mat &f0, Mat &f1){
        double sum = 0.0;


        for(int i = 0; i < RSsize; i++)
        {
            for(int j = 0; j < RSsize; j++)
            {
                //sum +=  (gauss.at<double>(i,j) * std::pow( int(f1.at<uchar>( (i+iws) , (j+jws) )) - int(f0.at<uchar>( (i+irs), (j+jrs) )) , 2));

                sum +=  (std::pow( int(f1.at<uchar>( (i+iws) , (j+jws) )) - int(f0.at<uchar>( (i+irs), (j+jrs) )) , 2));
            }
        }
        return ( (sum) );
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

    //namedWindow("frame0",1);
    //namedWindow("frame1",1);
    for (iws = iwsMin; iws < iwsMax; ++iws)
    {
        for (jws = jwsMin; jws < jwsMax; ++jws)
        {
            if(iws < 0 || jws < 0 || (iws + SRsize) > frame1.rows || (jws + SRsize) > frame1.cols) {
               // cout << "continuo con: (" << iws << "," << jws << ")\n";
                continue;
            }
            /*
            Mat newframe0;
            Mat newframe1;

            frame0.copyTo(newframe0);
            frame1.copyTo(newframe1);
            cout << "iws: " << iws << " jws: " << jws << endl;
            cout << "isr: " << isr << " jsr: " << jsr << endl;
            rectangle(newframe0, Point(jsr-1,isr-1),Point(jsr+SRsize,isr+SRsize), Scalar( 0 , 0, 255 ),1);
            rectangle(newframe1, Point(jwsMin-1,iwsMin-1),Point(jwsMin+WSsize,iwsMin+WSsize), Scalar( 255 , 0, 0 ),1);
            rectangle(newframe1, Point(jws-1,iws-1),Point(jws+SRsize,iws+SRsize), Scalar( 0 , 255, 0 ),1);
            */


            tmp = MSE(isr, jsr, iws, jws, SRsize, frame0, frame1);
            /*
            cout << "MSE:  " << tmp << endl;

            imshow("frame0",newframe0);
            imshow("frame1",newframe1);

            waitKey(0);
            */
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
        double i = (Actual.first - it->first) * (Actual.first - it->first);
        double j = (Actual.second - it->second) * (Actual.second - it->second);

        double distance = std::sqrt(i+j);


        if(distance < min) {
            PixelOp = std::make_pair(it->first, it->second);
            min = distance;
         }
    }
    /*
    cout << "PixelOP X:" << PixelOp.first -((SRsize-1)/2) << " Y: " << PixelOp.second -((SRsize-1)/2) << endl;
    cout << "----------------------------" << endl;
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

    Gauss(SRsize);


    Mat RGBframe0;
    Mat RGBframe1;
    Mat frame0;
    Mat frame1;

    video.read(RGBframe0);
    cvtColor(RGBframe0, frame0, CV_BGR2GRAY);

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

    while(video.read(RGBframe1))
    {

        cvtColor(RGBframe1, frame1, CV_BGR2GRAY);

        for (std::map<pixel,pixel>::iterator Map_it = MovimentMap.begin(); Map_it != MovimentMap.end(); ++Map_it)
        {
                pixel ActualPixel = Map_it->second;

                PixelOp = std::move( Match (ActualPixel , SRsize, WSsize, frame0, frame1 )) ;

                Map_it->second = PixelOp;

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

RayFlux RaysExtractor::Normalize(const RayFlux rays,const int SizeNorm)
{
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

class RaysExtractorOLBP: public RaysExtractor
{
    public:
        MapRaysFlux Extract(VideoCapture &,const int, const int, const bool);
};

MapRaysFlux RaysExtractorOLBP::Extract(VideoCapture &video, const int SRsize, const int WSsize, const bool visual = false)
{


    if(!video.isOpened()){
        cout << "Video file error" << endl;
        MapRaysFlux RaysRoi;
        return(RaysRoi);
    }

    Mat frame0;
    Mat frame1;
    Mat lbpframe0;
    Mat lbpframe1;

    video.read(frame0);
    lbpframe0 = Mat::zeros(frame0.rows-2, frame0.cols-2, CV_8UC1);
    lbpframe1 = Mat::zeros(frame0.rows-2, frame0.cols-2, CV_8UC1);

    //cout << frame0.channels() << endl;
    //cout << lbp::OLBP(frame0).channels() << endl;

    lbp::OLBP(frame0, lbpframe0);

    map<pixel,pixel> MovimentMap; //Mapa que permite ver a cual pixel se movio en el siguiente tiempo (t+1)
    //En el caso del primer pixel parte con un movimiento a si mismo

    for(int i = (SRsize-1)/2; i < lbpframe0.rows - (SRsize-1)/2; i++ )
    {
        for(int j = (SRsize-1)/2; j < lbpframe0.cols -(SRsize-1)/2; j++)
        {
            pixel ActualPixel = make_pair(i,j);
            MovimentMap[ActualPixel] = ActualPixel;
        }
    }

    MapRaysFlux  RaysMap;
    pixel PixelOp;

    while(video.read(frame1))
    {
        lbp::OLBP(frame1, lbpframe1);

        for (std::map<pixel,pixel>::iterator Map_it = MovimentMap.begin(); Map_it != MovimentMap.end(); ++Map_it)
        {
                pixel ActualPixel = Map_it->second;
                PixelOp = std::move( RaysExtractorOLBP::Match (ActualPixel , SRsize, WSsize, lbpframe0, lbpframe1 )) ;
                Map_it->second = PixelOp;
                RaySupport newRaySupport;

                if(visual)
                    newRaySupport = std::make_pair( float(PixelOp.first), float(PixelOp.second)  );
                else
                     newRaySupport = std::make_pair( float(ActualPixel.first - PixelOp.first), float(ActualPixel.second - PixelOp.second)  );

                RaysMap[Map_it->first].push_back(newRaySupport);
        }
        lbpframe1.copyTo(lbpframe0);
    }

    return(RaysMap);
}