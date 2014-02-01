#include "histogram.h"

Histogram::Histogram()
{
}


cv::MatND Histogram::getHistogramValues(const cv::Mat &image, const int &histSize) const
{
    //histSize: depending on number of bins
    //example: range = 0...255 -> 256 values, bins: 16 -> histSize = range/bins = 256/16 = 16
    cv::MatND hist;

    cv::calcHist(&image,
                 1,          //1 image only
                 0,          //channels
                 cv::Mat(),  //no mask
                 hist,       //destination
                 1,          //1D histogram
                 &histSize,  //number of bins
                 &histRange, //range of values
                 true,
                 false);
    return hist;
}

cv::Mat Histogram::getHistogramImage(const cv::Mat &image, const int &histSize) const
{
    cv::MatND hist = getHistogramValues(image, histSize);     //get HistogramValues and save in hist

    double max = 0;
    double min = 0;
    cv::minMaxLoc(hist, &min, &max);

    cv::Mat histImg(histSize, histSize, CV_8UC1, cv::Scalar(255));

    for(int i = 0; i < histSize; i++)
    {
        float value = hist.at<float>(i);
        int intensity = static_cast<int>(value/max * 0.9*histSize);

        cv::line(histImg, cv::Point(i,histSize), cv::Point(i,histSize-intensity), cv::Scalar(0));
    }

    return histImg;
}


