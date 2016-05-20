#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
using namespace cv;
using namespace std;

//遍历图像mat, 对每个像素做加操作

Mat srcImg, src;
int thresh = 0;
int max_thresh = 255;
char* source_window = "Source image";

void ScanImageAndReduceC(int, void*)
{
	src = srcImg;
 	// accept only char type matrices
    CV_Assert(src.depth() == CV_8U);
    const int channels = src.channels();
    switch(channels)
    {
    case 1:
        {
            for( int i = 0; i < src.rows; ++i)
                for( int j = 0; j < src.cols; ++j )
                    src.at<uchar>(i,j) = src.at<uchar>(i,j) + thresh;
            break;
        }
    case 3:
        {
         Mat_<Vec3b> _I = src;
         for( int i = 0; i < src.rows; ++i)
            for( int j = 0; j < src.cols; ++j )
               {
                   _I(i,j)[0] = _I(i,j)[0] + thresh;
                   _I(i,j)[1] = _I(i,j)[1] + thresh;
                   _I(i,j)[2] = _I(i,j)[2] + thresh;
            }
         src = _I;
         break;
        }
    }
   imshow( source_window, src );
}

int main( int, char** argv )
{
	srcImg = imread( argv[1], 1 );
	if( srcImg.empty() )
	{ 
		cout<<"Usage: ./mat_add"<<endl;
		return -1;
	}
	namedWindow( source_window, CV_WINDOW_KEEPRATIO );
	createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, ScanImageAndReduceC );
	ScanImageAndReduceC(0,0);
	waitKey(0);

	return 0;
}