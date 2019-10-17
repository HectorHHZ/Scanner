#ifndef EDGEDETECT_H
#define EDGEDETECT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <math.h>
#include <algorithm>
#include <iostream>

struct houghpixel {
    int rho;
    int theta;
    int val;
};

struct line {
    float m; // The slope of the line.
    float b; // The intersection with y axis.
    float c; // If line is verticle, x = c.
};

class edgedetect
{
public:
    edgedetect(std::string filepath);

    cv::Point2f top_left_corner;
    cv::Point2f top_right_corner;
    cv::Point2f bottom_left_corner;
    cv::Point2f bottom_right_corner;

    void debug();
    void process();

private:
    cv::Mat src;
    cv::Mat gray_image;
    cv::Mat blur_image;
    cv::Mat contour;
    cv::Mat hough_space;
    cv::Mat hough_test;
    std::vector<houghpixel> houghlines;
    std::vector<line> lines;
    std::vector<cv::Point2f> corners;

    const int Gaussiansize = 13;
    const int GRAD_THRESHOLD = 50;
    const int HOUGH_THRESHOLD = 100;
    const int LINE_DISTANCE = 75;

    float houghDistance(houghpixel a, houghpixel b);

    void preprocess();
    void contourDetect();
    void transToHoughSpace();
    bool selectLineInHoughSpace();
    void transBackToOriginSpace();
    void findIntersections();
    void displayLinesAndCorners();
    void sortCorners();
};

#endif // EDGEDETECT_H
