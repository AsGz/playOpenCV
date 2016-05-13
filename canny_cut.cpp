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

RNG rng(12345);


bool addToVertexs(vector<Point2f> &vertexs, Point2f p)
{
    float near = 2.0;
    bool tooNear = false;
    for(int i=0; i<vertexs.size(); i++)
    {
        if( abs(p.x-vertexs.at(i).x) < near && abs(p.y - vertexs.at(i).y) < near  )
        {
            tooNear = true;
            break;
        }
    }
    if (!tooNear) vertexs.push_back(p);
    return !tooNear;
} 

void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
  std::vector<cv::Point2f> top, bot;
  for (int i = 0; i < corners.size(); i++)
  {
    if (corners[i].y < center.y)
      top.push_back(corners[i]);
    else
      bot.push_back(corners[i]);
  }
  corners.clear();
  
  if (top.size() == 2 && bot.size() == 2)
  {
    cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
    cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
    cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
    cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
    
    corners.push_back(tl);
    corners.push_back(tr);
    corners.push_back(br);
    corners.push_back(bl);
  }
}

void doCutUV(vector<Point2f> &vertexs)
{
  cv::Point2f center(0,0);
  for (int i = 0; i < vertexs.size(); i++)
    center += vertexs[i];
  center *= (1. / vertexs.size());

  sortCorners(vertexs, center);
  if (vertexs.size() == 0)
  {
    std::cout << "The corners were not sorted correctly!" << std::endl;
    return;
  }

  cv::Mat quad = cv::Mat::zeros(794, 714, CV_8UC3);
  std::vector<cv::Point2f> quad_pts;
  quad_pts.push_back(cv::Point2f(0, 0));
  quad_pts.push_back(cv::Point2f(quad.cols, 0));
  quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
  quad_pts.push_back(cv::Point2f(0, quad.rows));

  cv::Mat transmtx = cv::getPerspectiveTransform(vertexs, quad_pts);
  cv::warpPerspective(src, quad, transmtx, quad.size());

  cv::imshow("cut uv", quad);
}

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
    /// Reduce noise with a kernel 3x3
    //blur( src_gray, src_gray, Size(3,3) );

    // Canny detector
    Canny( src_gray, detected_edges, lowThreshold, lowThreshold*nratio, kernel_size );
    
    //threshold(detected_edges, detected_edges, lowThreshold, 255, THRESH_BINARY);
    
    dst = Scalar::all(0);
    src.copyTo( dst, detected_edges);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)  );
    
    // Get the moments
    vector<Moments> mu(contours.size() );
    //  Get the mass centers:
    vector<Point2f> mc( contours.size()  );
    //用来保存识别的顶点
    vector<Point2f> vertexs; 
    for(int i=0; i<contours.size(); i++)
    {
        int h = i;
        int c = 0;
        while(hierarchy.at(h)[2] != -1)
        {
            h = hierarchy.at(h)[2];
            c++;
        }
        mu[i] = moments( contours[i], false  );
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00  );
        if( c >= 7 && c < 10 )
        {
            cout << i << " hierarchy size:" << c << endl;
            //cout << i << "moment point :" << mc[i] << endl;
            if ( addToVertexs(vertexs, mc[i]) )
            {
                //测试展示
                Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255)  );
                drawContours( dst, contours, i, color, 2, 8, hierarchy, 0, Point()  );
            }
        }
    }
    //测试展示
    for (int j=0; j<vertexs.size(); j++)
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255)  );
        circle( dst, vertexs.at(j), 4, color, -1, 8, 0  );
        cout << j << "vertexs point :" << vertexs.at(j) << endl;
    }
    doCutUV(vertexs);
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

  //GaussianBlur(src, src, Size(5,5), 0);

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, CV_WINDOW_NORMAL );

  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }
