#ifndef FILTER_H
#define FILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cmath>
#include <algorithm>
#include <iostream>

cv::Mat adjust_contrast(cv::Mat src, double alpha, int beta);
cv::Mat adjust_brightness(cv::Mat src, double alpha, int beta);
cv::Mat shapening(cv::Mat src);
cv::Mat colorized_his_eql(cv::Mat src);
cv::Mat multi_scale_sharpen(cv::Mat src);
cv::Mat handwritting_enhance(cv::Mat src);

#endif // FILTER_H
