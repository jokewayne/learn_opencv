#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>

using std::cout; using std::cerr; using std::endl;
using std::vector;

std::string getString(const int n) {
	std::stringstream newstr;
	newstr<<n;
	return newstr.str();
}

cv::Mat getMat(cv::Mat img, cv::Size sz) { 
	int channels;
	if ( 1 == img.channels()) {
		return cv::Mat::zeros(sz.height, sz.width, CV_8UC1);
	} else if ( 3 == img.channels()) {
		return cv::Mat::zeros(sz.height, sz.width, CV_8UC3);
	} 
	return cv::Mat::zeros(sz.height, sz.width, CV_8UC3);
}

void viewMat(cv::Mat img, int col, int row) {

	if ( col >= img.cols || row >= img.rows || col < 0 || row < 0) {
		cout << "col:" << col
		     << ", row:" << row
                     << "; img.cols:" << img.cols
   		     << ", img.rows:" << img.rows << endl;
	} else {
		cout << "BGR:(" << row << ", " << col << ")=[" 
		     << getString(img.at<cv::Vec3b>(row, col)[0]) << ","
		     << getString(img.at<cv::Vec3b>(row, col)[1]) << ","
		     << getString(img.at<cv::Vec3b>(row, col)[2]) << "]" << endl;
	} 
}

cv::Mat findblob(cv::Mat src) {

	// Setup SimpleBlobDetector parameters.
	cv::SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 0;
	params.maxThreshold = 200;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 50;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.2;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.1;

	// Storage for blobs
	vector<cv::KeyPoint> keypoints;

	// Set up detector with params
	cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);   

	// Detect blobs
	detector->detect( src, keypoints);
	cv::Mat im_with_keypoints;
	drawKeypoints( src, keypoints, im_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );	
	return im_with_keypoints;	
}


cv::Mat mysubtract(cv::Mat src1, cv::Mat src2) {
	cv::Mat out = cv::Mat::zeros(480,640, CV_8UC1);
	//cv::absdiff(src1, src2, out);
	for ( int y=0; y < out.rows; y++) {
		for( int x=0; x < out.cols; x++ ) {
			out.at<uchar>(y,x) = abs(src1.at<cv::Vec3b>(y,x)[0] - src2.at<cv::Vec3b>(y,x)[0]);
			//cout << "[" << x << "," << y << "]:" << (int)out.at<uchar>(y,x) << endl; 
			if ( out.at<uchar>(y,x) > 50 ) {
				out.at<uchar>(y,x)=255;
			}  else {
				out.at<uchar>(y,x)=0;
			} 

		}
	}
	return out;
}

cv::Mat colorMouse(cv::Mat img, int colortype, int sv) {
	cv::Mat hsvimg; 
	cv::Mat out = cv::Mat(img);	
	cv::cvtColor(img, hsvimg, cv::COLOR_BGR2HSV,3);
	int h = 0, s = 0, v = 0, hc =0;
	int pointx = 0, pointy =0;
	int count = 0;
	for (int y=0; y < out.rows; y++ ) {
		for ( int x = 0; x < out.cols; x++ ) {
			h = hsvimg.at<cv::Vec3b>(y,x)[0]; 
			s = hsvimg.at<cv::Vec3b>(y,x)[1];
			v = hsvimg.at<cv::Vec3b>(y,x)[2];
			if ( s < sv || v < sv ) {
				continue;
			}
			if ( h > 15 ) { 
				hc = (h-15)/30;
			} else {
				hc = 5; 
			}
			//hc 0=GR 1=G 2=BG 3=B 4=BR 5=R
			// [0]=B [1]=G [2]=R
			if ( colortype == hc ) {
				count++;
				pointx += x;
				pointy += y;
			}
			switch (hc) {
				// yellow 
				case 0:
					out.at<cv::Vec3b>(y,x)[1] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 255;
					out.at<cv::Vec3b>(y,x)[0] = 0;
					break;
				// green
				case 1:
					out.at<cv::Vec3b>(y,x)[1] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					out.at<cv::Vec3b>(y,x)[0] = 0;
					break;
				// sky blue 
				case 2:
					out.at<cv::Vec3b>(y,x)[1] = 255;
					out.at<cv::Vec3b>(y,x)[0] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					break;
				// blue
				case 3:
					out.at<cv::Vec3b>(y,x)[0] = 255;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					break;
				// purple
				case 4:
					out.at<cv::Vec3b>(y,x)[0] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 255;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					break;
				// red
				case 5:
					out.at<cv::Vec3b>(y,x)[2] = 255;
					out.at<cv::Vec3b>(y,x)[0] = 0;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					break;
				default:
					out.at<cv::Vec3b>(y,x)[0] = 0;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					break;
			}	
		}
	}
	cv::Point center(pointx/count, pointy/count);
	circle(out, center, 10, cv::Scalar(255,255,255), 3, 8, 0);
	return out;
}


cv::Mat hsvtoColor(cv::Mat img) {
	cv::Mat hsvimg; 
	cv::Mat out = cv::Mat(img);	
	cv::cvtColor(img, hsvimg, cv::COLOR_BGR2HSV,3);
	int h = 0, s = 0, v = 0, hc =0;
	for (int y=0; y < out.rows; y++ ) {
		for ( int x = 0; x < out.cols; x++ ) {
			h = hsvimg.at<cv::Vec3b>(y,x)[0]; 
			s = hsvimg.at<cv::Vec3b>(y,x)[1];
			v = hsvimg.at<cv::Vec3b>(y,x)[2];
			if ( s < 43 || v < 46 ) {
				continue;
			}
			if ( h > 15 ) { 
				hc = (h-15)/30;
			} else {
				hc = 5; 
			}
			//hc 0=GR 1=G 2=BG 3=B 4=BR 5=R
			// [0]=B [1]=G [2]=R
			switch (hc) {
				case 0:
					out.at<cv::Vec3b>(y,x)[1] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 255;
					out.at<cv::Vec3b>(y,x)[0] = 0;
					break;
				case 1:
					out.at<cv::Vec3b>(y,x)[1] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					out.at<cv::Vec3b>(y,x)[0] = 0;
					break;
				case 2:
					out.at<cv::Vec3b>(y,x)[1] = 255;
					out.at<cv::Vec3b>(y,x)[0] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					break;
				case 3:
					out.at<cv::Vec3b>(y,x)[0] = 255;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					break;
				case 4:
					out.at<cv::Vec3b>(y,x)[0] = 255;
					out.at<cv::Vec3b>(y,x)[2] = 255;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					break;
				case 5:
					out.at<cv::Vec3b>(y,x)[2] = 255;
					out.at<cv::Vec3b>(y,x)[0] = 0;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					break;
				default:
					out.at<cv::Vec3b>(y,x)[0] = 0;
					out.at<cv::Vec3b>(y,x)[1] = 0;
					out.at<cv::Vec3b>(y,x)[2] = 0;
					break;
			}	
		}
	}
	return out;
}

cv::Mat alphaBlend(cv::Mat img1, cv::Mat img2, double alpha) {
	cv::Mat out = cv::Mat::zeros(img1.rows, img1.cols, CV_8UC3);
	if ( alpha > 1 ) {
		return img1;
	}
	double beta = 1 - alpha;
	for ( int y=0; y < img1.rows; y++ ) {
		for ( int x=0; x < img1.cols; x++ ) {
			if ( x >= img2.cols || y >= img2.rows ) {
				out.at<cv::Vec3b>(y,x)[0] = img1.at<cv::Vec3b>(y,x)[0];
				out.at<cv::Vec3b>(y,x)[1] = img1.at<cv::Vec3b>(y,x)[1];
				out.at<cv::Vec3b>(y,x)[2] = img1.at<cv::Vec3b>(y,x)[2];
			} else {
				out.at<cv::Vec3b>(y,x)[0] = (int) ( alpha * (double)img1.at<cv::Vec3b>(y,x)[0] + beta * (double)img2.at<cv::Vec3b>(y,x)[0]);
				out.at<cv::Vec3b>(y,x)[1] = (int) ( alpha * (double)img1.at<cv::Vec3b>(y,x)[1] + beta * (double)img2.at<cv::Vec3b>(y,x)[1]);
				out.at<cv::Vec3b>(y,x)[2] = (int) ( alpha * (double)img1.at<cv::Vec3b>(y,x)[2] + beta * (double)img2.at<cv::Vec3b>(y,x)[2]);
			}
		}
	}
	return out;
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


