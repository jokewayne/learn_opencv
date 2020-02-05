#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>  // cv::Canny()
#include <iostream>
#include <math.h>
#include "mylib.hpp"

using namespace cv;
using std::cout; using std::cerr; using std::endl;
/* modified by me */
const String capture_name = "MyCamera";
const String window_name = "MyConfig";
const String info_name = "info";
const String ptype_name = "Process Type";
const String p1_name = "P1";
const String p2_name = "P2";
const String p3_name = "P3";
const String p4_name = "P4";
int ptype = 0;
int p1 = 0;
int p2 = 0;
int p3 = 0;
int p4 = 0;
int psize=0;
double thresh = 100;
int maxVal = 255;
int count = 0;
int powercount = 1;
Mat frame,  processed, infoimg, configimg, background, cpimg;

static void on_ptype_trackbar(int, void *)
{
   setTrackbarPos(ptype_name, window_name, ptype); 
}
static void on_p1_trackbar(int, void *)
{
   setTrackbarPos(p1_name, window_name, p1); 
}
static void on_p2_trackbar(int, void *)
{
   setTrackbarPos(p2_name, window_name, p2); 
}
static void on_p3_trackbar(int, void *)
{
   setTrackbarPos(p3_name, window_name, p3); 
}
static void on_p4_trackbar(int, void *)
{
   setTrackbarPos(p4_name, window_name, p4); 
}
static void do_process() 
{
	psize = p1 * 2 + 1;
	switch(ptype)
	{
		case 0:
    			cv::Canny(frame, processed, 400, 1000, 5);
			break;
		case 1:
			cv::GaussianBlur(frame, processed, cv::Size(psize,psize),3,3);
			break;
		case 2:
			cv::blur(frame, processed, Size(psize,psize), Point(-1,0));
			break;
		case 3:
			cv::medianBlur( frame, processed, psize);
			break;
		case 4: 
			cv::cvtColor(frame, processed, CV_BGR2GRAY);
			break;
		case 5:
			cv::cvtColor(frame, processed, CV_BGR2GRAY);
			cv::threshold(processed, processed, (double)p4, maxVal, cv::THRESH_BINARY);
			break;
		case 6:
			cv::flip(frame, processed, p2);
			break;
		case 7:
			processed = channel_swap(frame);
			break;
		case 8:
			processed = BGR2GRAY(frame);
			infoimg = histinfo(processed);
			break;
		case 9:
			processed = BGR2GRAY(frame);
			processed = grayInvert(processed);
			break;
		case 10:
			processed = frame;
			infoimg = histinfo(processed);
			break;
		case 11:
			processed = myrotate(frame, p1*36.0, 1);
			break;
		case 12:
			processed = alphaBlend(frame, background, (double)p1/10); 
			break;
		case 13:
			processed = frame;
			viewMat(processed, p1, p2);
			break;
		case 14:
			cpimg = alphaBlend(frame, cpimg, (double)1/count);
			processed = cpimg;
			if ( count % 3 == 0) {
				cout << "save picture: " << getString(count) << endl;
				imwrite("cpimg13_" + getString(count) + ".jpg", cpimg);
				if ( powercount < 4 )  powercount++;
			}
			count++;
			cout << "count: " << getString(count) << endl;
			break;
		default:
			processed=frame;
			break;
	}
}

int main(int, char**)
{
    cout << "Opening camera..." << endl;
    VideoCapture capture(0); // open the first camera
    if (!capture.isOpened())
    {
        cerr << "ERROR: Can't initialize camera capture" << endl;
        return 1;
    }
    namedWindow(capture_name);
    namedWindow(window_name);
    namedWindow(info_name);

    configimg = cv::Mat::zeros(100, 400, CV_8UC1);
    infoimg = cv::Mat::zeros(300, 300, CV_8UC1);
    cpimg = cv::Mat::zeros(480, 640, CV_8UC3);
    background = imread("ng2.jpg");
    cout << "Frame width: " << capture.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << "     height: " << capture.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "Capturing FPS: " << capture.get(CAP_PROP_FPS) << endl;

    cout << endl << "Press 'ESC' to quit, 'space' to toggle frame processing" << endl;
    cout << endl << "Start grabbing..." << endl;

    size_t nFrames = 0;
    bool enableProcessing = false;
    int64 t0 = cv::getTickCount();
    int64 processingTime = 0;

    createTrackbar(ptype_name, window_name, &ptype, 20, on_ptype_trackbar);
    createTrackbar(p1_name, window_name, &p1, 10, on_p1_trackbar);
    createTrackbar(p2_name, window_name, &p2, 10, on_p2_trackbar);
    createTrackbar(p3_name, window_name, &p3, 10, on_p3_trackbar);
    createTrackbar(p4_name, window_name, &p4, 250, on_p4_trackbar);


    for (;;)
    {
        capture >> frame; // read the next frame from camera
        if (frame.empty())
        {
            cerr << "ERROR: Can't grab camera frame." << endl;
            break;
        }
        nFrames++;
        if (nFrames % 10 == 0)
        {
            const int N = 10;
            int64 t1 = cv::getTickCount();
            cout << "ptype:" << ptype
		 << "-p1:" << p1 
		 << "-p2:" << p2 
		 << "-p3:" << p3 
		 << "-p4:" << p4 
		 << "TotalFrames:" << cv::format("%5lld", (long long int)nFrames)
                 << "-Average FPS: " << cv::format("%9.1f", (double)getTickFrequency() * N / (t1 - t0))
                 << "-Average time per frame: " << cv::format("%9.2f ms", (double)(t1 - t0) * 1000.0f / (N * getTickFrequency()))
                 << "-Average processing time: " << cv::format("%9.2f ms", (double)(processingTime) * 1000.0f / (N * getTickFrequency()))
                 << std::endl;
            t0 = t1;
            processingTime = 0;
        }
        if (!enableProcessing)
        {
            imshow(capture_name, frame);
            imshow(window_name, configimg);
    	    moveWindow(window_name, 0, 0 ); 
	}
        else
        {
            int64 tp0 = cv::getTickCount();
            //cv::Canny(frame, processed, 400, 1000, 5);
	    do_process();
            processingTime += cv::getTickCount() - tp0;
            imshow(capture_name, processed);
	    imshow(window_name, configimg);
	    imshow(info_name, infoimg);
        }
        moveWindow(info_name, 1366 - infoimg.cols, 0);
	moveWindow(window_name, 0, 0 ); 
        int key = waitKey(1);
        if (key == 27/*ESC*/)
            break;
        if (key == 32/*SPACE*/)
        {
            enableProcessing = !enableProcessing;
            cout << "Enable frame processing ('space' key): " << enableProcessing << endl;
        }
	if ( key == 83 || key == 115 ) {
		imwrite("img_"+getString(count)+".jpg",processed);	
		count++;
	} 
    }
    std::cout << "Number of captured frames: " << nFrames << endl;
    return nFrames > 0 ? 0 : 1;
}
