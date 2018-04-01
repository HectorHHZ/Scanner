#include "edgedetect.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <math.h>
#include <algorithm>

//Constructor
edgedetect::edgedetect(std::string filepath)
{
    Gaussiansize.width = Gaussiansize.height = 13;
    GRAD_THRESHOLD = 50;
    HOUGH_THRESHOLD = 100;
    LINE_DISTANCE = 75;

    src = cv::imread(filepath);
    gray_image = blur_image = contour = src.clone();

    hough_space = cv::Mat(distance(src.rows, src.cols), 360, src.type());
    process();
}

void edgedetect::process() {
    // TODO: Iteratively choose the pramaters
    changeToGray();
    blur();
    contourDetect();
    transform();
    selectLine();
    transBack();
    displayLine();
    displayCorner();
    warpped = warp();
}

void edgedetect::debug() {
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
    cvNamedWindow("Warp", 0);
    cvResizeWindow("Warp", 500, 500);
    cv::imshow("Warp", warpped);
}

bool edgedetect::houghPixelSort(houghpixel &a, houghpixel &b) {
    return (a.val >= b.val);
}

float edgedetect::distance(float x, float y) {
    return sqrt(x * x + y * y);
}

float edgedetect::houghDistance(houghpixel a, houghpixel b) {
    return (distance(a.rho - b.rho, a.theta - b.theta));
}

void edgedetect::changeToGray() {
    cv::cvtColor(src, gray_image, CV_RGB2GRAY);
    cv::cvtColor(hough_space, hough_space, CV_RGB2GRAY);
    hough_test = hough_space.clone();
}

void edgedetect::blur() {
    cv::GaussianBlur(gray_image, blur_image, Gaussiansize, 0, 0);
}

void edgedetect::contourDetect() {
    cv::Mat dx, dy;
    cv::Mat abs_dx, abs_dy;

    // Calculate gradients on x and y directions.

    // TODO: Detect algorithm need to be improved.

    cv::Sobel(blur_image, dx, CV_16S, 1, 0, 3, 1, 1);
    cv::convertScaleAbs(dx, abs_dx);
    cv::Sobel(blur_image, dy, CV_16S, 0, 1, 3, 1, 1);
    cv::convertScaleAbs(dy, abs_dy);

    // Combine the gradients together.
    cv::addWeighted(abs_dx, 0.5, abs_dy, 0.5, 0, contour);
}

void edgedetect::transform() {
    // Go through all the pixels.
    for (int i = 1; i < contour.rows; i++) {
        for (int j = 1; j < contour.cols; j++) {
            // Only select the salient contour pixels.

            /* BUG: Different grad cause the contour be unprecise
             * Ieratively select the GRAD_THRESHOLD.
             */

            if (contour.at<uchar>(i, j) >= GRAD_THRESHOLD) {
                // Calculate corresponding coordinates in hough space.
                for (int theta = -180; theta <= 180; theta++) {
                    int rho = i * sin(CV_PI / 180 * theta)
                            + j * cos(CV_PI / 180 * theta);
                    if (rho >= 0 && rho <= hough_space.rows) {
                        hough_space.at<uchar>(rho, theta + 180) += 1;
                    }
                }
            }
        }
    }
}

void edgedetect::selectLine() {
    // Construct a vector to store the nominated pixels.
    std::vector<houghpixel> pixels;
    for (int i = 1; i < hough_space.rows; i++) {
        for (int j = 1; j < hough_space.cols; j++) {
            if (hough_space.at<uchar>(i, j) >= HOUGH_THRESHOLD) {
                houghpixel pixel;
                pixel.rho = i;
                pixel.theta = j;
                pixel.val = hough_space.at<uchar>(i, j);
                pixels.push_back(pixel);
            }
        }
    }

    // Sort the pixels based on the brightness.
    std::sort(pixels.begin(), pixels.end(), houghPixelSort);

    // Select four most possible lines.

    /* TODO: Algorithm should detect whether the
     * selected lines can form a quadrangle.
     */

    // NOTE: Can use clustering to select the four region first.
    houghlines.push_back(pixels.front());
    int count = 1;
    for (unsigned int i = 1; i < pixels.size(); i++) {
        bool test = true;
        for (unsigned int j = 0; j < houghlines.size(); j++) {
            // Test whether two lines are too close.
            // TODO: Iteratively select the LINE_DISTANCE.
            if (houghDistance(pixels[i], houghlines[j]) < LINE_DISTANCE) {
                test = false;
                break;
            }
        }
        if (test == true) {
            houghlines.push_back(pixels[i]);
            count += 1;
        }
        if (count == 4) {break;}
    }

    // Test code, show the selected four lines.
    for (unsigned int i = 0; i < houghlines.size(); i++) {
        hough_test.at<uchar>(houghlines[i].rho, houghlines[i].theta)
                += houghlines[i].val;
    }
}

void edgedetect::transBack() {
    // Transfer the point in houghspace to original image space.
    for (unsigned int i = 0; i < houghlines.size(); i++) {
        float m = -cos(CV_PI / 180 * (houghlines[i].theta - 180)) /
                  sin(CV_PI / 180 * (houghlines[i].theta - 180));
        float b = houghlines[i].rho / sin(CV_PI / 180 * (houghlines[i].theta - 180));
        line l;
        l.m = m;
        l.b = b;
        lines.push_back(l);
    }
}

void edgedetect::displayLine() {
    // BUG: Cannot dealing with vertical lines
    for (unsigned int i = 0; i < lines.size(); i++) {
        cv::line(src, cv::Point2f(0, lines[i].b),
                 cv::Point2f(src.rows, src.rows * lines[i].m + lines[i].b),
                 cv::Scalar(90, 90, 90), 3);
    }
}

void edgedetect::displayCorner() {
    // Find the four intersections.

    /* TODO: Algorithm is need to select exactly four correct corners.
     * The four point should be within the image.
     */

    for (unsigned int i = 0; i < lines.size(); i++) {
        for (unsigned int j = i + 1; j <= lines.size(); j++) {
            float x = (lines[i].b - lines[j].b) / (lines[j].m - lines[i].m);
            float y = lines[i].m * x + lines[i].b;
            cv::Point2f p(x, y);
            if (x > 10 && x < src.cols - 10 && y > 10 && y < src.rows - 10) {
                corners.push_back(p);
            }
        }
    }

    // Display the four intersection.
    for (unsigned int i = 0; i < corners.size(); i++) {
        cv::circle(src, corners[i], 1, cv::Scalar(255, 0, 0), 5);
    }
}

cv::Mat edgedetect::warp() {
    // Sort the four corners.
    // TODO: The sort algorithm need to be improved.
    std::vector<cv::Point2f> sort_corner;
    while (sort_corner.size() < 4) {
        for (unsigned int i = 0; i < corners.size(); i++) {
            if (corners[i].x < src.cols / 2 && corners[i].y < src.rows / 2) {
                sort_corner.push_back(corners[i]);
            }
        }
        for (unsigned int i = 0; i < corners.size(); i++) {
            if (corners[i].x > src.cols / 2 && corners[i].y < src.rows / 2) {
                sort_corner.push_back(corners[i]);
            }
        }
        for (unsigned int i = 0; i < corners.size(); i++) {
            if (corners[i].x < src.cols / 2 && corners[i].y > src.rows / 2) {
                sort_corner.push_back(corners[i]);
            }
        }
        for (unsigned int i = 0; i < corners.size(); i++) {
            if (corners[i].x > src.cols / 2 && corners[i].y > src.rows / 2) {
                sort_corner.push_back(corners[i]);
            }
        }
    }

    // TODO: Design the size of the warpped image.

    //Warp the image.
    cv::Mat new_image = cv::Mat::zeros(700, 500, CV_8UC3);
    std::vector<cv::Point2f> new_corners;
    new_corners.push_back(cv::Point2f(0, 0));
    new_corners.push_back(cv::Point2f(500, 0));
    new_corners.push_back(cv::Point2f(0, 700));
    new_corners.push_back(cv::Point2f(500, 700));

    cv::Mat transmtx = cv::getPerspectiveTransform(sort_corner, new_corners);
    cv::warpPerspective(src, new_image, transmtx, new_image.size());

    return new_image;
}