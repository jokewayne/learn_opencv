#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using std::cout; using std::cerr; using std::endl;

cv::Mat getMat(cv::Mat img, cv::Size sz) { 
	int channels;
	if ( 1 == img.channels()) {
		return cv::Mat::zeros(sz.height, sz.width, CV_8UC1);
	} else if ( 3 == img.channels()) {
		return cv::Mat::zeros(sz.height, sz.width, CV_8UC3);
	} 
	return cv::Mat::zeros(sz.height, sz.width, CV_8UC3);
}
cv::Mat myrotate(cv::Mat img, double degree, double resized) {
	cv::Mat rotmat = getRotationMatrix2D(cv::Point(img.cols/2, img.rows/2), degree, resized);
	double rad = degree/180. * M_PI;
	int width = resized * (img.cols * std::cos(rad) + img.rows * std::sin(rad));
	int height = resized * (img.cols * std::sin(rad) + img.rows * std::cos(rad)); 
	int type = img.type();
	cv::Mat out = cv::Mat::zeros(height, width, type);
	cv::warpAffine(img, out, rotmat, out.size());
	return out;
}
cv::Mat mycut(cv::Mat img, cv::Point2i p, cv::Size sz) {
	int img_width, img_height, out_width, out_height;
	int channels = img.channels();
	if ( p.x < 0 || p.y < 0 || p.x >= img.cols || p.y >= img.rows ) {
		return cv::Mat(0,0,CV_8UC3);
	}
	img_width = img.cols;
	img_height = img.rows;
	out_width = sz.width;
	out_height = sz.height;
	if ( img_width - p.x < out_width ) {
		out_width = img_width - p.x;
	}
	if ( img_height - p.y < out_height) {
		out_height = img_height - p.y;
	}
	cv::Mat out = getMat(img, cv::Size(out_height, out_width));
	for ( int x = 0; x < out_width; x++ ) {
		for ( int y = 0; y < out_height; y++) {
			if ( 1 == channels ) {
				out.at<uchar>(y,x) = img.at<uchar>(y+p.y, x+p.x);	
			} else if ( 3 == channels ) {
				out.at<cv::Vec3b>(y,x)[0] = img.at<cv::Vec3b>(y+p.y, x+p.x)[0];
				out.at<cv::Vec3b>(y,x)[1] = img.at<cv::Vec3b>(y+p.y, x+p.x)[1];
				out.at<cv::Vec3b>(y,x)[2] = img.at<cv::Vec3b>(y+p.y, x+p.x)[2];
			}
		}	
	}
	return out;
}

cv::Mat myflood(cv::Mat img, cv::Scalar src, int floodrange, cv::Scalar dst) {
	cv::Mat out = getMat(img, cv::Size(img.rows, img.cols));
	int channels = img.channels();
	if ( 1 == channels ) {
		for ( int x = 0; x < out.cols; x++ ) {
			for ( int y = 0; y < out.rows; y++ ) {
				if ( img.at<uchar>(y,x) < src.val[0] - floodrange || img.at<uchar>(y,x) > src.val[0] + floodrange ) {
					continue;
				} else {
					out.at<uchar>(y,x) = dst.val[0];
				}				
			}
		}
	} else if ( 3 == channels ) {
		for ( int x = 0; x < out.cols; x++ ) {
			for ( int y = 0; y < out.rows; y++ ) {
				if (    img.at<cv::Vec3b>(y,x)[0] < src.val[0] - floodrange || \
					img.at<cv::Vec3b>(y,x)[0] > src.val[0] + floodrange || \
					img.at<cv::Vec3b>(y,x)[1] < src.val[1] - floodrange || \
					img.at<cv::Vec3b>(y,x)[1] > src.val[1] + floodrange || \
					img.at<cv::Vec3b>(y,x)[2] < src.val[2] - floodrange || \
					img.at<cv::Vec3b>(y,x)[2] > src.val[2] + floodrange  ) {
						continue;
				} else {
					out.at<cv::Vec3b>(y,x)[0] = dst.val[0];
					out.at<cv::Vec3b>(y,x)[1] = dst.val[1];
					out.at<cv::Vec3b>(y,x)[2] = dst.val[2];
				}
			}
		}
	}
	return out;
}


// affine
cv::Mat myaffine(cv::Mat img, double a, double b, double c, double d, double tx, double ty, double theta){
  // get height and width
  int width = img.cols;
  int height = img.rows;
  int channel = img.channels();

  // get detriment
  double det = a * d - b * c;

  if (theta != 0){
    // Affine parameters
    double rad = theta / 180. * M_PI;
    a = std::cos(rad);
    b = - std::sin(rad);
    c = std::sin(rad);
    d = std::cos(rad);
    tx = 0;
    ty = 0;

    double det = a * d - b * c;

    // center transition
    double cx = width / 2.;
    double cy = height / 2.;
    double new_cx = (d * cx - b * cy) / det;
    double new_cy = (- c * cx + a * cy) / det;
    tx = new_cx - cx;
    ty = new_cy - cy;
  }

  // Resize width and height
  int resized_width = (int)(width * a);
  int resized_height = (int)(height * d);
  
  if (theta != 0) {
    resized_width = (int)(width);
    resized_height = (int)(height);
  }

  // other parameters
  int x_before, y_before;
  double dx, dy, wx, wy, w_sum;
  double val;
  int _x, _y;

  // output
  cv::Mat out = cv::Mat::zeros(resized_height, resized_width, CV_8UC3);

  // Affine transformation
  for (int y = 0; y < resized_height; y++){    
    for (int x = 0; x < resized_width; x++){

      // get original position x
      x_before = (int)((d * x - b * y) / det - tx);

      if ((x_before < 0) || (x_before >= width)){
        continue;
      }

      // get original position y
      y_before = (int)((-c * x + a * y) / det - ty);

      if ((y_before < 0) || (y_before >= height)){
        continue;
      }

      // assign pixel to new position
      for (int c = 0; c < channel; c++){
        out.at<cv::Vec3b>(y, x)[c] = img.at<cv::Vec3b>(y_before, x_before)[c];
      }
    }
  }

  return out;
}

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


