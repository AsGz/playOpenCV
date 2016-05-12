#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;
/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold=90;
int const max_lowThreshold = 100;
int nratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*nratio, kernel_size );

  dst = Scalar::all(0);
  src.copyTo( dst, detected_edges);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)  );
    for(int i=0; i<contours.size(); i++)
    {
        int h = i;
        int c = 0;
        while(hierarchy.at(h)[2] != -1)
        {
            h = hierarchy.at(h)[2];
            c++;
        }
        if( c >= 5 )
        {
            cout << i << " hierarchy size:" << c << endl;
            drawContours( dst, contours, i, Scalar(255,255,0), 2, 8, hierarchy, 0, Point()  );
        }
    } 


  /// Using Canny's output as a mask, we display our result
    imshow( window_name, dst );
 }


/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( argv[1] );

  if( !src.data )
  { return -1; }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  //GaussianBlur(src, src, Size(3,3), 0);

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }
