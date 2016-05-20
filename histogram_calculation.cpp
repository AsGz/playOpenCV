
//计算图片的直方图
//直方图的均衡化

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

void showBGRHist(Mat &src, vector<Mat>& bgr_planes,  string &name)
{
  int histSize = 256;
  float range[] = { 0, 256 } ;
  const float* histRange = { range };
  bool uniform = true; bool accumulate = false;
  Mat b_hist, g_hist, r_hist;
  if (bgr_planes.size() > 0)  calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
  if (bgr_planes.size() > 1) calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
  if (bgr_planes.size() > 2) calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
 
  //imshow("blue color plane", bgr_planes[0]);
  //imshow("green color plane", bgr_planes[1]);
  //imshow("red color plane", bgr_planes[2]);
  
  // Draw the histograms for B, G and R
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );
  
  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  
  //数值根据展示区域等比缩小(最大数不超过展示区域的rows)
  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 4, 0  );
      //rectangle(histImage, Point(i-1, histImage.cols), Point(i-1, cvRound(b_hist.at<float>(i-1))),Scalar(255,0,0), 2, 4, 0 ); 
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 4, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 4, 0  );
  }
  
  //namedWindow("calcHist Demo", WINDOW_AUTOSIZE );
  
  imshow(name + " hist", histImage );
  imshow(name, src);
}    

int main( int, char** argv )
{
  Mat src, dst;
  src = imread( argv[1], 1 );
  if( src.empty() ) { return -1; }
 
  vector<Mat> bgr_planes;
  //按照Blue、Green、Red进行分割
  split( src, bgr_planes );
  
  string name= "befor equalization";
  showBGRHist(src, bgr_planes, name);

  for(int i=0; i<bgr_planes.size(); i++)
  {
    equalizeHist( bgr_planes[i], bgr_planes[i] );
  }

  name = "after qualization";
  merge(bgr_planes, dst);
  showBGRHist(dst, bgr_planes,  name);
  

  waitKey(0);
  return 0;
}
