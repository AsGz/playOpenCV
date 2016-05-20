#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
using namespace cv;
using namespace std;
int main( int, char** argv )
{
  Mat src, dst;
  const char* source_window = "Source image";
  const char* equalized_window = "Equalized Image";
  src = imread( argv[1], 1 );
  if( src.empty() )
  { 
      cout<<"Usage: ./Histogram_Demo <path_to_image>"<<endl;
      return -1;
  }
  vector<Mat> bgr_planes;
  //按照Blue、Green、Red进行分割
  split( src, bgr_planes );
 
  for(int i=0; i<bgr_planes.size(); i++)
  {
    equalizeHist( bgr_planes[i], bgr_planes[i] );
  }
  //把转化后的通道合并
  merge(bgr_planes, dst);
  namedWindow( source_window, WINDOW_AUTOSIZE );
  namedWindow( equalized_window, WINDOW_AUTOSIZE );
  imshow( source_window, src );
  imshow( equalized_window, dst );
  waitKey(0);
  return 0;
}
