#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


//CenterX => cols
//CenterY => Rows
namespace utility{

	class ROI
	{
		private:
			int X_;
			int Y_;
			int Height_;
			int Width_; 
			int CenterX_; 
			int CenterY_;

		public:
			ROI(const int X,const int Y,const int Width,const int Height,const int CenterX,const int CenterY){
				X_       = X;
				Y_       = Y;
				Height_  = Height;
				Width_   = Width;
				CenterX_ = CenterX;
				CenterY_ = CenterY;
			}
			ROI(){};

			int GetX_() const { return (X_); }
			int GetY_() const { return (Y_); }
			int GetWidth_() const { return (Width_); }
			int GetHeight_() const { return (Height_); }
			int ColsBegin() const{ return (X_); }
			int RowsBegin() const { return (Y_); }
			int ColsEnd() const { return (X_ + Width_); }
			int RowsEnd() const { return (Y_ + Height_); }
			int CenterX() const { return (CenterX_); }
			int CenterY() const { return (CenterY_); }
			void operator=(const ROI &tmp){
				X_      = tmp.GetX_();
				Y_      = tmp.GetY_();
				Height_ = tmp.GetHeight_();
				Width_  = tmp.GetWidth_();
				CenterX_ = tmp.CenterX();
				CenterY_ = tmp.CenterY();
			}
	};


	//Funcion para crear ROI
	//i -> rows
	//j -> cols
	Mat CreateROI(const Mat& I,const int i, const int j,const int size){ //size impar

		int d = (size - 1)/2;

		int x   = (j - d) < 0 ? 0 : j - d;
		int w   = 1 + (j - x) + ( (j + d) > I.cols - 1 ? (I.cols - 1 - j) : d );

		int y   = (i - d) < 0 ? 0 : i - d;
		int h   = 1 + (i - y) + ( (i + d) > I.rows - 1 ? (I.rows - 1 - i) : d );

		Rect roi(x,y,w,h);

		return I(roi);
	}


	ROI CreateRoi(const Mat& I,const int i, const int j,const int size){ //size impar

		int d = (size - 1)/2;

		int x   = (j - d) < 0 ? 0 : j - d;
		int w   = 1 + (j - x) + ( (j + d) > I.cols - 1 ? (I.cols - 1 - j) : d );

		int y   = (i - d) < 0 ? 0 : i - d;
		int h   = 1 + (i - y) + ( (i + d) > I.rows - 1 ? (I.rows - 1 - i) : d );

		ROI roi(x,y,w,h,i,j);

		return (roi);
	}
}

/*
int main(int argc, char const *argv[])
{
	Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
 
    cout << "Rows: " << image.rows << endl;
    cout << "Cols: " << image.cols << endl;


	ROI roi;
    roi = CreateRoi(image,50,50,5);


    cout << roi.GetX_() << endl;
    cout << roi.GetY_() << endl;

    for (int i = roi.RowsBegin(); i < roi.RowsEnd(); ++i){
    	for (int j = roi.ColsBegin(); j < roi.ColsEnd(); ++j){
    		cout << (int)image.at<uchar>(i,j) << " ";
    	}cout << endl;
    }

//    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
//    imshow( "Display window", roi );       

	waitKey(0);
	return 0;
}*/