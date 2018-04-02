#ifndef EDGEDETECT_H
#define EDGEDETECT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <math.h>
#include <algorithm>

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
    cv::Mat src;
    cv::Mat gray_image;
    cv::Mat blur_image;
    cv::Mat contour;
    cv::Mat hough_space;
    cv::Mat hough_test;
    cv::Mat warpped;
    std::vector<houghpixel> houghlines;
    std::vector<line> lines;
    std::vector<cv::Point2f> corners;

    cv::Size Gaussiansize;
    int GRAD_THRESHOLD;
    int HOUGH_THRESHOLD;
    int LINE_DISTANCE;

    edgedetect(std::string filepath);

    void process();
    void debug();
    float distance(float x, float y);
    float houghDistance(houghpixel a, houghpixel b);

    void changeToGray();
    void blur();
    void contourDetect();
    void transToHoughSpace();
    void selectLineInHoughSpace();
    void transBackToOriginSpace();
    void findIntersections();
    void displayLinesAndCorners();
    cv::Mat warp();

    static bool cmp_houghPixelSort(houghpixel &a, houghpixel &b);
};

#endif // EDGEDETECT_H
