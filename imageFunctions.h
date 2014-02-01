#ifndef MATRIX_H
#define MATRIX_H

#define _USE_MATH_DEFINES
#include "histogram.h"
#include <QtCore>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>


class EndoscopeData
{
public:
    double rotation;
    double bending;
    unsigned int quadrant;

    explicit EndoscopeData();
    explicit EndoscopeData(const double &rot, const double &bend, const unsigned int &quadr);
    ~EndoscopeData();

    friend QDebug& operator<<(QDebug& os, const EndoscopeData data);
};


class Matrix : public cv::Mat
{

private:
    bool firstInit;
    int edgeThresh;
    unsigned int minRadius;
    unsigned int maxRadius;
    unsigned int radius;
    QPoint center;
    Matrix(const Matrix &);

public:
    void removeBlackFrame();
    bool getROI(QList<cv::Point> &massCenters);
    void getMassCenters(const std::vector<std::vector<cv::Point> > &contours, QList<cv::Point> &massCenters) const;
    void getRotation(const cv::Point &massCenter, EndoscopeData &angleBefore, EndoscopeData &angleAfter, EndoscopeData &motor) const;
    void getBending(const cv::Point &massCenter, EndoscopeData &angleBefore, EndoscopeData &angleAfter, EndoscopeData &motor) const;
    void getMotorData(const EndoscopeData &angle, EndoscopeData &angleBefore, EndoscopeData &angleAfter, EndoscopeData &motor) const;


    //! default constructor
    Matrix() : cv::Mat(),
        firstInit(true),
        edgeThresh(50),
        radius(0)
    {}
    //! constructs 2D matrix of the specified size and type
    // (_type is CV_8UC1, CV_64FC3, CV_32SC(12) etc.)
    Matrix(int rows, int cols, int type) : cv::Mat(rows, cols, type) {}
    Matrix(cv::Size size, int type) : cv::Mat(size, type) {}
    //! constucts 2D matrix and fills it with the specified value _s.
    Matrix(int rows, int cols, int type, const cv::Scalar& s) : cv::Mat(rows, cols, type, s) {}
    Matrix(cv::Size size, int type, const cv::Scalar& s) : cv::Mat(size, type, s) {}

    //! constructs n-dimensional matrix
    Matrix(int ndims, const int* sizes, int type) : cv::Mat(ndims, sizes, type) {}
    Matrix(int ndims, const int* sizes, int type, const cv::Scalar& s) : cv::Mat(ndims, sizes, type, s) {}

    //! copy constructor
    Matrix(const Mat& m) : cv::Mat(m) {}
    //! constructor for matrix headers pointing to user-allocated data
    Matrix(int rows, int cols, int type, void* data, size_t step=AUTO_STEP) : cv::Mat(rows, cols, type, data, step)
    {}
    Matrix(cv::Size size, int type, void* data, size_t step=AUTO_STEP) : cv::Mat(size, type, data, step) {}
    Matrix(int ndims, const int* sizes, int type, void* data, const size_t* steps=0) : cv::Mat(ndims, sizes, type, data, steps) {}

    //! creates a matrix header for a part of the bigger matrix
    Matrix(const cv::Mat& m, const cv::Range& rowRange, const cv::Range& colRange=cv::Range::all()) : cv::Mat(m, rowRange, colRange) {}
    Matrix(const cv::Mat& m, const cv::Rect& roi) : cv::Mat(m, roi) {}
    Matrix(const cv::Mat& m, const cv::Range* ranges) : cv::Mat(m, ranges) {}
    //! converts old-style CvMat to the new matrix; the data is not copied by default
    Matrix(const CvMat* m, bool copyData=false) : cv::Mat(m, copyData) {}
    //! converts old-style CvMatND to the new matrix; the data is not copied by default
    Matrix(const CvMatND* m, bool copyData=false) : cv::Mat(m, copyData) {}
    //! converts old-style IplImage to the new matrix; the data is not copied by default
    Matrix(const IplImage* img, bool copyData=false) : cv::Mat(img, copyData) {}
    //! builds matrix from std::vector with or without copying the data
    template<typename _Tp> explicit Matrix(const std::vector<_Tp>& vec, bool copyData=false) : cv::Mat(vec, copyData) {}
    //! builds matrix from cv::Vec; the data is copied by default
    template<typename _Tp, int n> explicit Matrix(const cv::Vec<_Tp, n>& vec, bool copyData=true) : cv::Mat(vec, copyData) {}
    //! builds matrix from cv::Matx; the data is copied by default
    template<typename _Tp, int m, int n> explicit Matrix(const cv::Matx<_Tp, m, n>& mtx, bool copyData=true) : cv::Mat(mtx, copyData) {}
    //! builds matrix from a 2D point
    template<typename _Tp> explicit Matrix(const cv::Point_<_Tp>& pt, bool copyData=true) : cv::Mat(pt, copyData) {}
    //! builds matrix from a 3D point
    template<typename _Tp> explicit Matrix(const cv::Point3_<_Tp>& pt, bool copyData=true)  : cv::Mat(pt, copyData) {}
    //! builds matrix from comma initializer
    template<typename _Tp> explicit Matrix(const cv::MatCommaInitializer_<_Tp>& commaInitializer) : cv::Mat(commaInitializer) {}
    //! download data from GpuMat
    explicit Matrix(const cv::gpu::GpuMat& m) : cv::Mat(m) {}

    ~Matrix();
};



#endif // MATRIX_H

