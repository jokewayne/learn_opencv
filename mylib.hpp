cv::Mat myaffine(cv::Mat img, double a, double b, double c, double d, double tx, double ty, double theta);
 cv::Mat BGR2GRAY(cv::Mat img);
cv::Mat grayInvert(cv::Mat img);
cv::Mat channel_swap(cv::Mat img);
cv::Mat histinfo(cv::Mat img);
cv::Mat mycut(cv::Mat img, cv::Point2i p, cv::Size sz);
cv::Mat myflood(cv::Mat img, cv::Scalar src, int floodrange, cv::Scalar dst);
cv::Mat myrotate(cv::Mat img, double degree, double resized) ;
cv::Mat alphaBlend(cv::Mat img1, cv::Mat img2, double alpha) ;

