#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


//Funcion para crear ROI
Mat CreateROI(const Mat& I,Mat* roi,const int i, const int j,const int size){ //size impar
	if(size % 2){
		//return NULL;
	}

	cout << "CHAO" << endl;
	int d = (size - 1)/2;

	int x = (i - d) < 0 ? 0 : i - d;
	int y = (j - d) < 0 ? 0 : j - d;
	int w = (i + d) > I.cols - 1 ? (I.cols - 1 - i) + d : i + d;
	int h = (j + d) > I.rows - 1 ? (I.rows - 1 - j) + d: j + d;


	cout << "x: " << x << endl;
	cout << "y: " << y << endl;
	cout << "w: " << w << endl;
	cout << "h: " << h << endl; 

	Rect ROI(x,y,w,h);

	//*roi = I(ROI);
	return I(ROI);
}


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

   /* int size = 89;
    int i = 100;
    int j = 200;

    int d = (size - 1)/2;

	


	cout << "x: " << x << endl;
	cout << "y: " << y << endl;
	cout << "w: " << w << endl;
	cout << "h: " << h << endl; 

	Rect Roi(x,y,w,h);
	Mat ROI = image(Roi);
	namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
	imshow( "Display window", ROI );       
*/


	Mat ROI;
    ROI = CreateROI(image,&ROI,0,0,5);

    cout << "Rows: " << ROI.rows << endl;
    cout << "Cols: " << ROI.cols << endl;


    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", ROI );       

	waitKey(0);
	return 0;
}