#include <opencv2/core.hpp>
#include <iostream>
#include <cmath>

using std::cout; using std::cerr; using std::endl;

// Channel swap
cv::Mat channel_swap(cv::Mat img){
  // get height and width
  int width = img.cols;
  int height = img.rows;

  // prepare output
  cv::Mat out = cv::Mat::zeros(height, width, CV_8UC3);

  // each y, x
  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      // R -> B
      out.at<cv::Vec3b>(y, x)[0] = img.at<cv::Vec3b>(y, x)[2];
      // B -> R
      out.at<cv::Vec3b>(y, x)[2] = img.at<cv::Vec3b>(y, x)[0];
      // G -> G
      out.at<cv::Vec3b>(y, x)[1] = img.at<cv::Vec3b>(y, x)[1];
    }
  }

  return out;
}

cv::Mat BGR2GRAY(cv::Mat img){
  // get height and width
  int width = img.cols;
  int height = img.rows;

  // prepare output
  cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1);

  // each y, x
  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      // BGR -> Gray
      out.at<uchar>(y, x) = 0.2126 * (float)img.at<cv::Vec3b>(y, x)[2] \
        + 0.7152 * (float)img.at<cv::Vec3b>(y, x)[1] \
        + 0.0722 * (float)img.at<cv::Vec3b>(y, x)[0];
    }
  }

  return out;
}

cv::Mat grayInvert(cv::Mat img){
  // get height and width
  int width = img.cols;
  int height = img.rows;

  // prepare output
  cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1);

  // each y, x
  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      // BGR -> Gray
      out.at<uchar>(y, x) = 255 - img.at<uchar>(y, x) - 1;
    }
  }

  return out;
}

cv::Mat hist_C1(cv::Mat img) {
  // get height and width
  int width = img.cols;
  int height = img.rows;
  int c[256];
  int BLACK = 0, WHITE = 255;
  int hight = 600;
  // prepare output
  cv::Mat out = cv::Mat::zeros(hight, 256, CV_8UC1);
  for ( int i =0 ; i < 256; i++ ) {
	c[i] = 0;
  }
  // each y, x
  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      // BGR -> Gray
      c[img.at<uchar>(y, x)] = c[img.at<uchar>(y, x)] + 1;
    }
  }
  double total = height * width;
  int counttotal = 0;
  for ( int i =0 ; i < 256; i++ ) {
      counttotal += c[i];
  }
  int count = 0;
  int bgcolor = 0;
  for ( int x = 0; x < 256; x++) {
	count = round(hight * (1.0 - ((double)(c[x])/total)));	
        /**
	cout << " c[" << x << "] = " << c[x] << "; count = " << count 
             << "; total = " << total 
             << "; countotal = " << counttotal << endl;
       **/
	if ( x < 128) {
		bgcolor = WHITE;
	} else {
		bgcolor = BLACK;
	}
    	for( int y = 0; y < hight; y++) {
		if ( y <= count ) {
			out.at<uchar>(y,x) = bgcolor;	
		} else { 
			out.at<uchar>(y,x) = x;	
		}
    	}
  }
  return out;
}

cv::Mat hist_C3(cv::Mat img) {
  // get height and width
  int width = img.cols;
  int height = img.rows;
  int c[3][256];
  int BLACK = 0, WHITE = 255;
  int hight = 600;
  // prepare output
  cv::Mat out = cv::Mat::zeros(hight, 256*3, CV_8UC3);
  for ( int j = 0; j < 3; j++ ) {
  	for ( int i =0 ; i < 256; i++ ) {
		c[j][i] = 0;
  	}
  }
  // each y, x
  for ( int channel = 0 ; channel < 3; channel++ ) {
  	for (int y = 0; y < height; y++){
    		for (int x = 0; x < width; x++){
      			c[channel][img.at<cv::Vec3b>(y, x)[channel]] = c[channel][img.at<cv::Vec3b>(y, x)[channel]] + 1;
    		}
  	}
  }
  // start to draw hist image
  float total = height * width;
  int count = 0;
  int bgcolor = 0;
  int position_x = 0;
  for ( int channel = 0 ; channel < 3; channel++ ) {
  	for ( int x = 0; x < 256; x++) {
		count = (int) hight * (1.0 - ((float)(c[channel][x])/total));	
        /**
	cout << " c[" << x << "] = " << c[x] << "; count = " << count 
             << "; total = " << total 
       **/
		if ( x < 128) {
			bgcolor = WHITE;
		} else {
			bgcolor = BLACK;
		}
    		for( int y = 0; y < hight; y++) {
			position_x = x  + 256 * channel;
			if ( y <= count ) {
				out.at<cv::Vec3b>(y,position_x)[0] = bgcolor;	
				out.at<cv::Vec3b>(y,position_x)[1] = bgcolor;	
				out.at<cv::Vec3b>(y,position_x)[2] = bgcolor;	
			} else { 
				out.at<cv::Vec3b>(y,position_x)[channel] = x;	
			}
    		}
  	}
  }
  return out;
}


cv::Mat histinfo(cv::Mat img){
  // get height and width
  int channels = img.channels();
  // prepare output
  
  cv::Mat out = cv::Mat::zeros(10, 10, CV_8UC1);
  if (  1 == channels ) {
	out = hist_C1(img);
   } else if ( 3 == channels ) {
	out = hist_C3(img); 
   } else {
	out = cv::Mat::zeros(100, 100, CV_8UC1);
   }
   return out;
}


