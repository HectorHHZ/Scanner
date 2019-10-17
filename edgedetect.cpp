#include "edgedetect.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <math.h>
#include <algorithm>

#include <QDebug>

bool cmp_hough_val(houghpixel a, houghpixel b)
{
    return (a.val > b.val);
}

bool cmp_hough_theta(houghpixel a, houghpixel b)
{
    return (a.theta < b.theta);
}

bool cmp_corners_x(cv::Point2f a, cv::Point2f b)
{
    return (a.x < b.x);
}

/* Constructor */
edgedetect::edgedetect(std::string filepath)
{
    src = cv::imread(filepath);
    hough_space = cv::Mat::zeros(sqrt(src.rows * src.rows + src.cols * src.cols),
                          360, src.type());
    hough_test = cv::Mat::zeros(sqrt(src.rows * src.rows + src.cols * src.cols),
                                360, src.type());

    // Initialize the four corners.
    top_left_corner = cv::Point2f(30, 30);
    top_right_corner = cv::Point2f(src.cols - 30, 30);
    bottom_left_corner = cv::Point2f(30, src.rows - 30);
    bottom_right_corner = cv::Point2f(src.cols - 30, src.rows - 30);
}

void edgedetect::process()
{
    preprocess();
    //qDebug("h");
    contourDetect();
    //qDebug("hh");
    transToHoughSpace();
    //qDebug("hhh");
    if (selectLineInHoughSpace())
    {
        //qDebug("hhhh");
        transBackToOriginSpace();
        //qDebug("xx");
        findIntersections();
        //qDebug("xxx");
        displayLinesAndCorners();
        //qDebug("xxxx");
        sortCorners();
        //qDebug("xxxxx");
    }
}

void edgedetect::debug()
{
    cvNamedWindow("hough_space", 0);
    cvResizeWindow("hough_space", 720, 1000);
    cv::imshow("hough_space", hough_space);
    cvNamedWindow("hough_space_test", 0);
    cvResizeWindow("hough_space_test", 720, 1000);
    cv::imshow("hough_space_test", hough_test);
    cvNamedWindow("Src", 0);
    cvResizeWindow("Src", 1000, 1000);
    cv::imshow("Src", src);
    cvNamedWindow("Contour", 0);
    cvResizeWindow("Contour", 1000, 1000);
    cv::imshow("Contour", contour);
    cvNamedWindow("Gray", 0);
    cvResizeWindow("Gray", 1000, 1000);
    cv::imshow("Gray", gray_image);
}

float edgedetect::houghDistance(houghpixel a, houghpixel b)
{
    return sqrt(pow((a.rho - b.rho), 2) + pow((a.theta - b.theta), 2));
}

void edgedetect::preprocess()
{
    // Convert to gray image.
    cv::cvtColor(src, gray_image, CV_RGB2GRAY);
    cv::cvtColor(hough_space, hough_space, CV_RGB2GRAY);
    cv::cvtColor(hough_test, hough_test, CV_RGB2GRAY);

    // Use gaussian blur to get rid of noise.
    cv::GaussianBlur(gray_image, blur_image, cv::Size(Gaussiansize, Gaussiansize), 0, 0);
}

void edgedetect::contourDetect()
{
    cv::Mat dx, dy;
    cv::Mat abs_dx, abs_dy;

    // Calculate gradients on x and y directions.

    cv::Sobel(blur_image, dx, CV_16S, 1, 0, 3, 1, 1);
    cv::convertScaleAbs(dx, abs_dx);
    cv::Sobel(blur_image, dy, CV_16S, 0, 1, 3, 1, 1);
    cv::convertScaleAbs(dy, abs_dy);

    // Combine the gradients together.
    cv::addWeighted(abs_dx, 0.5, abs_dy, 0.5, 0, contour);

}

void edgedetect::transToHoughSpace()
{
    // Go through all the pixels.
    for (int i = 1; i < contour.rows; i++)
    {
        for (int j = 1; j < contour.cols; j++)
        {
            // Only select the salient contour pixels.
            if (contour.at<uchar>(i, j) >= GRAD_THRESHOLD)
            {
                // Calculate corresponding coordinates in hough space.
                for (int theta = -180; theta <= 180; theta++)
                {
                    int rho = i * sin(CV_PI / 180 * theta)
                            + j * cos(CV_PI / 180 * theta);
                    if (rho >= 0 && rho <= hough_space.rows &&
                            theta + 180 < hough_space.cols)
                    {
                        hough_space.at<uchar>(rho, theta + 180) += 1;
                    }
                }
            }
        }
    }
}

bool edgedetect::selectLineInHoughSpace()
{
    // Construct a vector to store the nominated pixels.
    std::vector<houghpixel> pixels;
    for (int i = 1; i < hough_space.rows; i++)
    {
        for (int j = 1; j < hough_space.cols; j++)
        {
            if (hough_space.at<uchar>(i, j) >= HOUGH_THRESHOLD)
            {
                houghpixel pixel;
                pixel.rho = i;
                pixel.theta = j;
                pixel.val = hough_space.at<uchar>(i, j);
                pixels.push_back(pixel);
            }
            else
            {
                hough_space.at<uchar>(i, j) = 0;
            }
        }
    }

    // Sort the pixels based on the brightness.
    std::sort(pixels.begin(), pixels.end(), cmp_hough_val);

    // Select four most possible lines.
    if (pixels.size() != 0)
    {
        houghlines.push_back(pixels.front());
        for (size_t i = 1; i < pixels.size(); i++)
        {
            bool good_point = true;
            for (size_t j = 0; j < houghlines.size(); j++)
            {
                // Test whether two lines are too close.
                if (houghDistance(pixels[i], houghlines[j]) < LINE_DISTANCE)
                {
                    good_point = false;
                    break;
                }
            }
            if (good_point == true)
            {
                houghlines.push_back(pixels[i]);
            }
        }
    }

    // Show the selected lines in hough_test space.
    for (size_t i = 0; i < houghlines.size(); i++)
    {
        if (houghlines[i].rho < hough_test.rows && houghlines[i].theta < hough_test.cols)
        {
            hough_test.at<uchar>(houghlines[i].rho, houghlines[i].theta)
                    += houghlines[i].val;
        }
    }

    // Deal with the situation when there are more than 4 pixels are selected.
    if (houghlines.size() > 4)
    {
        std::sort(houghlines.begin(), houghlines.end(), cmp_hough_val);
        while (houghlines.size() > 5)
        {
            houghlines.pop_back();
        }
        std::sort(houghlines.begin(), houghlines.end(), cmp_hough_theta);
        if (houghlines.size() == 5)
        {
            int dis01 = abs(houghlines[0].theta - houghlines[1].theta);
            int dis12 = abs(houghlines[1].theta - houghlines[2].theta);
            int dis23 = abs(houghlines[2].theta - houghlines[3].theta);
            int dis34 = abs(houghlines[3].theta - houghlines[4].theta);
            if (abs(dis01 - dis12) > 2)
            {
                // When one of the 0, 1, 2 is redundent.
                if (dis01 < dis12)
                {
                    // When 0 and 1 are pairs.
                    if (abs(dis23 - dis34) > 2)
                    {
                        if (dis23 < dis34)
                        {
                            // When 2 and 3 are pairs.
                            houghlines.erase(houghlines.begin() + 4);
                        }
                        else
                        {
                            // When 3 and 4 are pairs.
                            houghlines.erase(houghlines.begin() + 2);
                        }
                    }
                    else
                    {
                        // Theta of 2, 3, and 4 are very similar, i.e. these
                        // three lines are parallel.
                        if (houghlines[2].val < houghlines[3].val && houghlines[2].val < houghlines[4].val)
                        {
                            houghlines.erase(houghlines.begin() + 2);
                        }
                        else if (houghlines[3].val < houghlines[2].val && houghlines[3].val < houghlines[4].val)
                        {
                            houghlines.erase(houghlines.begin() + 3);
                        }
                        else
                        {
                            houghlines.erase(houghlines.begin() + 4);
                        }
                    }
                }
                else
                {
                    // When 1 and 2 are pairs.
                    houghlines.erase(houghlines.begin());
                }
            }
            else
            {
                // Theta of 0, 1, and 2 are very similar, i.e. these
                // three lines are parallel.
                if (houghlines[0].val < houghlines[1].val && houghlines[0].val < houghlines[2].val)
                {
                    houghlines.erase(houghlines.begin());
                } else if (houghlines[1].val < houghlines[0].val && houghlines[1].val < houghlines[2].val)
                {
                    houghlines.erase(houghlines.begin() + 1);
                }
                else
                {
                    houghlines.erase(houghlines.begin() + 2);
                }
            }
        }
    }
    else if (houghlines.size() < 4)
    {
        std::cout << "Error occured! Sorry" << std::endl;
        return false;
    }
    return true;
}

void edgedetect::transBackToOriginSpace()
{
    // Transfer the point in houghspace back to original image space.
    for (size_t i = 0; i < houghlines.size(); i++)
    {
        if (houghlines[i].theta == 0 || houghlines[i].theta == 180 || houghlines[i].theta == 360)
        {
            line l;
            l.c = houghlines[i].rho + 1;
            lines.push_back(l);
        }
        else
        {
            float m = -cos(CV_PI / 180 * (houghlines[i].theta - 180)) /
                      sin(CV_PI / 180 * (houghlines[i].theta - 180));
            float b = houghlines[i].rho / sin(CV_PI / 180 * (houghlines[i].theta - 180));
            line l;
            l.m = m;
            l.b = b;
            l.c = 0;
            lines.push_back(l);
        }
    }
    // Judge whether there are exactly four lines.
    if (lines.size() != 4)
    {
        std::cout << "Error occured in function selectLineInHoughSpace!" << std::endl;
        exit(-1);
    }
}

void edgedetect::findIntersections()
{
    // Find the four intersections.
    for (unsigned int i = 0; i < lines.size(); i++)
    {
        for (unsigned int j = i + 1; j < lines.size(); j++)
        {
            if (lines[i].c == 0 && lines[j].c == 0)
            {
                float x = (lines[i].b - lines[j].b) / (lines[j].m - lines[i].m);
                float y = lines[i].m * x + lines[i].b;
                if (x > 0 && x < src.cols && y > 0 && y < src.rows)
                {
                    cv::Point2f p(x, y);
                    corners.push_back(p);
                }
            }
            else if (lines[i].c != 0 && lines[j].c == 0)
            {
                float x = lines[i].c;
                float y = lines[j].m * x + lines[j].b;
                if (x > 0 && x < src.cols && y > 0 && y < src.rows)
                {
                    cv::Point2f p(x, y);
                    corners.push_back(p);
                }
            } else if (lines[i].c == 0 && lines[j].c != 0)
            {
                float x = lines[j].c;
                float y = lines[i].m * x + lines[i].b;
                if (x > 0 && x < src.cols && y > 0 && y < src.rows)
                {
                    cv::Point2f p(x, y);
                    corners.push_back(p);
                }
            }
            else
            {
                continue;
            }
        }
    }
    if (corners.size() != 4)
    {
        std::cout << "Error!" << std::endl;
        exit(-3);
    }
}

void edgedetect::displayLinesAndCorners()
{
    // Display the four lines.
    for (size_t i = 0; i < lines.size(); i++)
    {
        if (lines[i].c != 0)
        {
            cv::line(src, cv::Point2f(lines[i].c, 0),
                     cv::Point2f(lines[i].c, src.rows),
                     cv::Scalar(90, 90, 90), 3);
        }
        else
        {
            cv::line(src, cv::Point2f(0, lines[i].b),
                     cv::Point2f(src.cols, src.cols * lines[i].m + lines[i].b),
                     cv::Scalar(90, 90, 90), 3);
        }
    }
    // Display the four intersection.
    for (size_t i = 0; i < corners.size(); i++)
    {
        cv::circle(src, corners[i], 1, cv::Scalar(255, 0, 0), 5);
    }
}

void edgedetect::sortCorners()
{
    // Sort the four corners.
    float centerx = (corners[0].x + corners[1].x + corners[2].x + corners[3].x) / 4;
    float centery = (corners[0].y + corners[1].y + corners[2].y + corners[3].y) / 4;
    cv::circle(src, cv::Point2f(centerx, centery), 1, cv::Scalar(255, 0, 0), 5);
    std::vector<cv::Point2f> top;
    std::vector<cv::Point2f> bottom;
    std::sort(corners.begin(), corners.end(), cmp_corners_x);

    for (size_t i = 0; i < corners.size(); i++)
    {
        if (corners[i].y <= centery && top.size() < 2)
        {
            top.push_back(corners[i]);
        }
        else
        {
            bottom.push_back(corners[i]);
        }
    }

    if (top.size() != bottom.size())
    {
        exit(-4);
    }

    // Slightly adjust the corner's position.
    if (gray_image.at<uchar>(top[0].y, top[0].x) < 100)
    {
        top[0].x += 3;
        top[0].y += 3;
    }
    if (gray_image.at<uchar>(top[1].y, top[1].x) < 100)
    {
        top[1].x -= 3;
        top[1].y += 3;
    }
    if (gray_image.at<uchar>(bottom[0].y, bottom[0].x) < 100)
    {
        bottom[0].x += 3;
        bottom[0].y -= 3;
    }
    if (gray_image.at<uchar>(bottom[1].y, bottom[1].x) < 100)
    {
        bottom[1].x -= 3;
        bottom[1].y -= 3;
    }

    // Store the four sorted corners.
    top_left_corner = top[0];
    top_right_corner = top[1];
    bottom_left_corner = bottom[0];
    bottom_right_corner = bottom[1];
}
