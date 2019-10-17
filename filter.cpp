#include "filter.h"

cv::Mat adjust_contrast(cv::Mat src, double alpha, int beta)
{
    // Create result image.
    cv::Mat result = cv::Mat::zeros(src.size(), src.type());

    // Adjust the contrast.
    src.convertTo(result, -1, alpha, beta);

    return result;
}

cv::Mat adjust_brightness(cv::Mat src, double alpha, int beta)
{
    // Create result image.
    cv::Mat result = cv::Mat::zeros(src.size(), src.type());

    // Adjust the
    src.convertTo(result, -1, alpha, beta);

    return result;
}

cv::Mat shapening(cv::Mat src)
{
    // Create result image.
    cv::Mat result = cv::Mat::zeros(src.size(), src.type());

    // Create kernel for this process.
    cv::Mat kernel(3, 3, CV_32F, cv::Scalar(-1));

    // In order to keep the original color,
    // set the center pixel to [8.85~9.15].
    kernel.at<float>(1, 1) = 8.9;

    cv::filter2D(src, result, src.depth(), kernel);

    return result;
}

cv::Mat colorized_his_eql(cv::Mat src)
{
    // Split the image into three channels.
    cv::Mat imageRGB[3];
    cv::split(src, imageRGB);

    // Apply histogram equalization to each channel.
    for (int i = 0; i < 3; i++)
    {
        cv::equalizeHist(imageRGB[i], imageRGB[i]);
    }

    // Merge the three channels.
    cv::merge(imageRGB, 3, src);

    return src;
}

cv::Mat multi_scale_sharpen(cv::Mat src)
{
    cv::Mat B1;
    cv::Mat B2;
    cv::Mat B3;
    cv::Mat result = cv::Mat::zeros(src.size(), src.type());

    cv::GaussianBlur(src, B1, cv::Size(5, 5), 1, 1);
    cv::GaussianBlur(src, B2, cv::Size(9, 9), 2, 2);
    cv::GaussianBlur(src, B3, cv::Size(19, 19), 4, 4);

    for (int i = 0; i < src.rows; i++)
    {
        uchar* src_ptr = src.ptr<uchar>(i);
        uchar* result_ptr = result.ptr<uchar>(i);
        uchar* B1_ptr = B1.ptr<uchar>(i);
        uchar* B2_ptr = B2.ptr<uchar>(i);
        uchar* B3_ptr = B3.ptr<uchar>(i);

        for (int j = 0; j < src.cols; j++)
        {
            for (int k = 0; k < src.channels(); k++)
            {
                int D1 = src_ptr[3 * j + k] - B1_ptr[3 * j + k];
                int D2 = B1_ptr[3 * j + k] - B2_ptr[3 * j + k];
                int D3 = B2_ptr[3 * j + k] - B3_ptr[3 * j + k];
                int sgn = (D1 > 0) ? 1 : -1;
                result_ptr[3 * j + k] = cv::saturate_cast<uchar>((1 - 0.5 * sgn) * D1
                                                                 - 0.5 * D2 + 0.25 * D3
                                                                 + src_ptr[3 * j + k]);
            }
        }
    }

    return result;
}

cv::Mat handwritting_enhance(cv::Mat src)
{
    cv::Mat result = src.clone();
    cv::Mat gray;
    cv::Mat binary_1;
    cv::Mat binary_2;

    cv::cvtColor(src, gray, CV_RGB2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0, 0);

    cv::adaptiveThreshold(gray, binary_1, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 7);
    cv::adaptiveThreshold(gray, binary_2, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 3, 3);

    for (int i = 0; i < result.rows; i++)
    {
        for (int j = 0; j < result.cols; j++)
        {
            result.at<cv::Vec3b>(i, j)[0] = cv::saturate_cast<uchar>(result.at<cv::Vec3b>(i, j)[0] +
                    0.6 * binary_1.at<uchar>(i, j) - 1 * binary_2.at<uchar>(i, j));
            result.at<cv::Vec3b>(i, j)[1] = cv::saturate_cast<uchar>(result.at<cv::Vec3b>(i, j)[1] +
                    0.6 * binary_1.at<uchar>(i, j) - 1 * binary_2.at<uchar>(i, j));
            result.at<cv::Vec3b>(i, j)[2] = cv::saturate_cast<uchar>(result.at<cv::Vec3b>(i, j)[2] +
                    0.6 * binary_1.at<uchar>(i, j) - 1 * binary_2.at<uchar>(i, j));
        }
    }

    return result;
}
