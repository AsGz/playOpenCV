/*****************************************************************************
 *   ViewController.m
 ******************************************************************************
 *   by Kirill Kornyakov and Alexander Shishkov, 13th May 2013
 ******************************************************************************
 *   Chapter 12 of the "OpenCV for iOS" book
 *
 *   Applying Effects to Live Video shows how to process captured
 *   video frames on the fly.
 *
 *   Copyright Packt Publishing 2013.
 *   http://bit.ly/OpenCV_for_iOS_book
 *****************************************************************************/

#import "ViewController.h"
#import <mach/mach_time.h> 

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace cv;
using namespace std;
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

void doCutUV(vector<Point2f> &vertexs, Mat &src, Mat &cut)
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
    std::vector<cv::Point2f> quad_pts;
    quad_pts.push_back(cv::Point2f(0, 0));
    quad_pts.push_back(cv::Point2f(cut.cols, 0));
    quad_pts.push_back(cv::Point2f(cut.cols, cut.rows));
    quad_pts.push_back(cv::Point2f(0, cut.rows));
    
    cv::Mat transmtx = cv::getPerspectiveTransform(vertexs, quad_pts);
    cv::warpPerspective(src, cut, transmtx, cut.size());
    //cv::imshow("cut uv", quad);
}

void detectFromVideo(Mat &frame, Mat &cut)
{
    Mat detected;
    int lowThreshold = 90, nratio=3,kernel_size=3 ;
    Canny( frame, detected, lowThreshold, lowThreshold*nratio, kernel_size );
    std::vector<std::vector<cv::Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( detected, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)  );
    
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
            if ( addToVertexs(vertexs, mc[i]) )
            {
                //测试展示
                Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255)  );
                drawContours( frame, contours, i, color, 2, 8, hierarchy, 0, cv::Point()  );
            }
        }
    }
    
    cv::Point2f center(0,0);
    for (int i = 0; i < vertexs.size(); i++)
        center += vertexs[i];
    center *= (1. / vertexs.size());
    
    sortCorners(vertexs, center);
    if (vertexs.size() == 0)
    {
        //std::cout << "The corners were not sorted correctly!" << std::endl;
        return;
    }
    //测试展示
    for (int j=0; j<vertexs.size(); j++)
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255)  );
        int first = j;
        int second = j+1 == vertexs.size() ? 0 : j+1;
        line(frame, vertexs.at(first), vertexs.at(second), rng.uniform(0, 255), 4);
        circle( frame, vertexs.at(j), 4, color, -1, 8, 0  );
        cout << j << "vertexs point :" << vertexs.at(j) << endl;
    }
    //TODO: 进行截图
    cut = cv::Mat::zeros(715, 715, CV_8UC3);
    doCutUV(vertexs, frame, cut);
}



@interface ViewController ()

@end

@implementation ViewController

@synthesize imageView;
@synthesize startCaptureButton;
@synthesize toolbar;
@synthesize videoCamera;

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Initialize camera
    videoCamera = [[CvVideoCamera alloc]
                   initWithParentView:imageView];
    videoCamera.delegate = self;
    videoCamera.defaultAVCaptureDevicePosition =
                                AVCaptureDevicePositionBack;
    videoCamera.defaultAVCaptureSessionPreset =
                                AVCaptureSessionPreset640x480;
    videoCamera.defaultAVCaptureVideoOrientation =
                                AVCaptureVideoOrientationPortrait;
    videoCamera.defaultFPS = 30;
    
    isCapturing = NO;
}

- (NSInteger)supportedInterfaceOrientations
{
    // Only portrait orientation
    return UIInterfaceOrientationMaskPortrait;
}

-(IBAction)startCaptureButtonPressed:(id)sender
{
    [videoCamera start];
    isCapturing = YES;
}

-(IBAction)stopCaptureButtonPressed:(id)sender
{
    [videoCamera stop];
    isCapturing = NO;
}

//TODO: may be remove this code
static double machTimeToSecs(uint64_t time)
{
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    return (double)time * (double)timebase.numer /
                          (double)timebase.denom / 1e9;
}

// Macros for time measurements
#if 1
  #define TS(name) int64 t_##name = cv::getTickCount()
  #define TE(name) printf("TIMER_" #name ": %.2fms\n", \
    1000.*((cv::getTickCount() - t_##name) / cv::getTickFrequency()))
#else
  #define TS(name)
  #define TE(name)
#endif

- (void)processImage:(cv::Mat&)image
{
    cv::Mat inputFrame = image;
    Mat gray, cut;
    cvtColor( inputFrame, gray, CV_BGR2GRAY );
    
    detectFromVideo(gray, cut);
    
//    MatToUIImage(<#const cv::Mat &image#>)
    //TODO: 对3d模型贴纹理进行展示
    
    
    gray.copyTo(image);
}




- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    if (isCapturing)
    {
        [videoCamera stop];
    }
}

- (void)dealloc
{
    videoCamera.delegate = nil;
}

@end
