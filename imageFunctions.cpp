#include "imageFunctions.h"
#include <QtAlgorithms>



EndoscopeData::EndoscopeData()
    : rotation(0),
      bending(0),
      quadrant(0)
{
}

EndoscopeData::EndoscopeData(const double &rot, const double &bend, const unsigned int &quadr)
    : rotation(rot),
      bending(bend),
      quadrant(quadr)
{
}

EndoscopeData::~EndoscopeData()
{
}

QDebug& operator<<(QDebug &os, const EndoscopeData data)
{
    return os.nospace() << "[rot:" << data.rotation << "; bend:" << data.bending << "; quadrant:" << data.quadrant << "]";
}




static double square(double x) {
    return (x*x);
}

static bool sortFunction (const std::vector<cv::Point> &i, const std::vector<cv::Point> &j) {
    return (cv::contourArea(i) > cv::contourArea(j));
}



Matrix::~Matrix()
{
    this->release();
}


void Matrix::removeBlackFrame() {
    if (firstInit) {
        minRadius = 380*cols/1920;
        maxRadius = 900*cols/1920;
        center = QPoint(940*cols/1920,555*rows/1080);
        firstInit = false;
    }
    radius = 0;
    cv::Mat src_copy = this->clone();
    QList<unsigned int> radiusList;

    // Run the edge detector on grayscale
    cv::Canny(src_copy, src_copy, edgeThresh, edgeThresh*4, 3);
    for (int row = 9; row < rows-10; row += 10) {
        for (int col = 100*cols/1920; col < cols-100*cols/1920; col++) {
            if(src_copy.at<uchar>(row,col) == 255) {
                unsigned int distance = qSqrt(square(center.x()-col) + square(center.y()-row));
                if (distance <= maxRadius && distance >= minRadius) {
                    radiusList.append(distance);
                    break;
                }
            }
        }
    }
    if (radiusList.empty()) {
        radius = minRadius - cols/192;
        cv::Mat circle(cv::Size(cols,rows), CV_8UC1, cv::Scalar(255));
        cv::circle(circle, cv::Point(center.x(), center.y()), radius, cv::Scalar(0), CV_FILLED);
        *this += circle;
        cv::threshold(*this, src_copy, 0, 255, CV_THRESH_BINARY_INV);
        *this += src_copy;
    }
    else {
        QListIterator<unsigned int> itr(radiusList);
        while (itr.hasNext()) {
            radius += itr.next();
        }
        radius /= radiusList.size();
        radius -= 25*(maxRadius*cols)/(radius*1920);

        cv::Mat circle(cv::Size(cols,rows), CV_8UC1, cv::Scalar(255));
        cv::circle(circle, cv::Point(center.x(), center.y()), radius, cv::Scalar(0), CV_FILLED);
        *this += circle;
        cv::threshold(*this, src_copy, 0, 255, CV_THRESH_BINARY_INV);
        *this += src_copy;
    }
}


bool Matrix::getROI(QList<cv::Point> &massCenters) {
    cv::Mat src_copy = this->clone();
    unsigned int elementBig = 0;
    unsigned int elementSmall = 0;

    if (cols > 1500) {
        elementBig = 31;
        elementSmall = 12;
    }
    else if (cols > 1200) {
        elementBig = 21;
        elementSmall = 9;
    }
    else if (cols > 800) {
        elementBig = 15;
        elementSmall = 6;
    }
    else {
        elementBig = 12;
        elementSmall = 4;
    }

    cv::Mat elementRectBig = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(elementBig,elementBig));
    cv::Mat elementRectSmall = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(elementSmall,elementSmall));

    Histogram h;
    cv::MatND histValues = h.getHistogramValues(src_copy,128);

    QList<QPoint> histMaxList;
    QPoint histMax(0,0);
    QPoint firstValley(0,0);
    for(int i = 0; i < 50; i++) {                   //detecting global maximum [0-50]
        int value = histValues.at<float>(i);
        if (value > histMax.y()) {
            histMax.setX(i);
            histMax.setY(value);
            firstValley.setX(i);
            firstValley.setY(value);
        }
    }

    int prev4 = 0;
    int prev3 = 0;
    int prev2 = 0;
    int prev1 = 0;
    int next1 = 0;
    int next2 = 0;
    int next3 = 0;
    int next4 = 0;
    for(int i = 0; i < 50; i++) {           //detecting local maxima
        prev4 = (i<4) ? 0 : histValues.at<float>(i-4);
        prev3 = (i<3) ? 0 : histValues.at<float>(i-3);
        prev2 = (i<2) ? 0 : histValues.at<float>(i-2);
        prev1 = (i<1) ? 0 : histValues.at<float>(i-1);
        next1 = (i>49) ? 0 : histValues.at<float>(i+1);
        next2 = (i>48) ? 0 : histValues.at<float>(i+2);
        next3 = (i>47) ? 0 : histValues.at<float>(i+3);
        next4 = (i>46) ? 0 : histValues.at<float>(i+4);
        int currentValue = histValues.at<float>(i);
        if (currentValue > prev4
                && currentValue > prev3
                && currentValue >= prev2
                && currentValue >= prev1
                && currentValue >= next1
                && currentValue >= next2
                && currentValue > next3
                && currentValue > next4) {
            histMaxList.append(QPoint(i,currentValue));
        }
    }
    for (int i = 0; i < histMaxList.size();) {    //remove too small local maxima
        if(histMaxList[i].y() < histMax.y()/10) {
            histMaxList.removeAt(i);
        }
        else {
            i++;
        }
    }

    if (histMaxList.empty()) {
        histMaxList.append(histMax);
    }
    for (int i = histMaxList[0].x(); i < histMaxList[0].x()+8; i++) {
        int currentValue = histValues.at<float>(i);
        if (currentValue < firstValley.y()) {
            firstValley.setX(i);
            firstValley.setY(currentValue);
        }
    }

    cv::threshold(src_copy, src_copy, firstValley.x()*2, 255, CV_THRESH_BINARY_INV);
    cv::morphologyEx(src_copy, src_copy, cv::MORPH_OPEN, elementRectSmall);        //shoud be depending on radius
    cv::morphologyEx(src_copy, src_copy, cv::MORPH_CLOSE, elementRectBig);

    cv::namedWindow("contours", CV_WINDOW_NORMAL);
    cv::imshow("contours", src_copy);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> Hierarchy;

    /// Find contours
    cv::findContours(src_copy, contours, Hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    qSort(contours.begin(), contours.end(), sortFunction);   //areas nach größe sortieren [groß -> klein]

    double areaLimit = static_cast<double>(3000)*rows*cols/(static_cast<double>(1920*1080));
    for (unsigned int i = 0; i < contours.size(); i++) {
        int area = cv::contourArea(contours[i]);
        if (area < static_cast<int>(areaLimit)) {                      //should be depending on radius and resolution
            contours.erase(contours.begin()+i);
        }
        else {
            i++;
        }
    }

    if (contours.empty()) {
        return false;               //Fehler, da gefundene areas zu klein -> keine Motordaten stellen
    }

    massCenters.clear();
    getMassCenters(contours, massCenters);
    cv::circle(*this, cv::Point(massCenters[0].x,massCenters[0].y), 5, cv::Scalar(120), CV_FILLED);
    return true;
}


void Matrix::getMassCenters(const std::vector<std::vector<cv::Point> > &contours, QList<cv::Point> &massCenters) const {
    for (unsigned int i = 0; i < contours.size(); i++) {
        /// Get the moment
        cv::Moments moment = cv::moments(contours[i]);
        massCenters.append(cv::Point(moment.m10/moment.m00, moment.m01/moment.m00));
    }
}


void Matrix::getRotation(const cv::Point &massCenter, EndoscopeData &angleBefore, EndoscopeData &angleAfter, EndoscopeData &motor) const
{
    if(qSqrt(square(massCenter.x - center.x()) + square(massCenter.y - center.y())) >= 125*cols/1920) return;

    EndoscopeData angle;
    angle.rotation = qAtan(static_cast<double>(qAbs(massCenter.x-center.x()))/static_cast<double>(qAbs(massCenter.y-center.y()))) * 180/M_PI;                                      //Angle_Rot

    // Identifikation des Quadranten, in dem sich der POI befindet
    if(massCenter.x >= center.x())		//rechte hälfte
    {
        if(massCenter.y <= center.y())
        {                                    //rechts oben (1)
            angleAfter.quadrant = 1;
            if (angleBefore.quadrant == 4 && qAbs(massCenter.y - center.y()) < 125*rows/1080) return;
            angleAfter.rotation = angleBefore.rotation + angle.rotation/10;
            if(angleAfter.rotation < -34) {
                angleAfter.rotation = -34;
            }
            else if(angleAfter.rotation > 34) {
                angleAfter.rotation = 34;
            }
            motor.rotation = -17.755*angleAfter.rotation + 2810;
        }
        else
        {                                   //rechts unten (4)
            angleAfter.quadrant = 4;
            if (angleBefore.quadrant == 1 && qAbs(massCenter.y - center.y()) < 125*rows/1080) return;
            angleAfter.rotation = angleBefore.rotation - angle.rotation/10;
            if(angleAfter.rotation < -34) {
                angleAfter.rotation = -34;
            }
            else if(angleAfter.rotation > 34) {
                angleAfter.rotation = 34;
            }
            motor.rotation = -17.755*angleAfter.rotation + 2810;
        }
    }
    else       //linke hälfte
    {
        if(massCenter.y <= center.y())
        {                                   //links oben (2)
            angleAfter.quadrant = 2;
            if (angleBefore.quadrant == 3 && qAbs(massCenter.y - center.y()) < 125*rows/1080) return;
            angleAfter.rotation = angleBefore.rotation - angle.rotation/10;
            if(angleAfter.rotation < -34) {
                angleAfter.rotation = -34;
            }
            else if(angleAfter.rotation > 34) {
                angleAfter.rotation = 34;
            }
            motor.rotation = -17.755*angleAfter.rotation + 2810;
        }
        else                                //links unten (3)
        {
            angleAfter.quadrant = 3;
            if (angleBefore.quadrant == 2 && qAbs(massCenter.y - center.y()) < 125*rows/1080) return;
            angleAfter.rotation = angleBefore.rotation + angle.rotation/10;
            if(angleAfter.rotation < -34) {
                angleAfter.rotation = -34;
            }
            else if(angleAfter.rotation > 34) {
                angleAfter.rotation = 34;
            }
            motor.rotation = -17.755*angleAfter.rotation + 2810;
        }
    }
}


void Matrix::getBending(const cv::Point &massCenter, EndoscopeData &angleBefore, EndoscopeData &angleAfter, EndoscopeData &motor) const
{
    if(qSqrt(square(massCenter.x - center.x()) + square(massCenter.y - center.y())) >= 125*cols/1920) return;

    double k = 1300;   //Koeffizent für Abstand zwischen Endoskop und Nasenhöhle
    EndoscopeData angle;
    angle.bending = qAtan(static_cast<double>(qAbs(massCenter.y - center.y()))/k) * 180.0/M_PI;	//Angle_Bend


    // Identifikation des Quadranten, in dem sich der POI befindet
    if(massCenter.x >= center.x())		//rechte hälfte
    {
        if(massCenter.y <= center.y())	//rechts oben(1)
        {
            angleAfter.quadrant = 1;
            if (angleBefore.quadrant == 3) return;
            angleAfter.bending = angleBefore.bending - angle.bending;
            if(angleAfter.bending < -60)
            {
                angleAfter.bending = -60;
            }
            else if(angleAfter.bending > 60)
            {
                angleAfter.bending = 60;
            }
            motor.bending = 5.75*angleAfter.bending + 2384;
        }
        else							//rechts unten(4)
        {
            angleAfter.quadrant = 4;
            angleAfter.bending = angleBefore.bending + angle.bending;
            if(angleAfter.bending < -60)
            {
                angleAfter.bending = -60;
            }
            else if(angleAfter.bending > 60)
            {
                angleAfter.bending = 60;
            }
            motor.bending = 5.432*angleAfter.bending + 2415;
        }
    }
    else								//linke hälfte
    {
        if(massCenter.y <= center.y())	//links oben(2)
        {
            angleAfter.quadrant = 2;
            angleAfter.bending = angleBefore.bending - angle.bending;
            if(angleAfter.bending < -60)
            {
                angleAfter.bending = -60;
            }
            else if(angleAfter.bending > 60)
            {
                angleAfter.bending = 60;
            }
            motor.bending = 5.75*angleAfter.bending + 2384;
        }
        else							//links unten(3)
        {
            angleAfter.quadrant = 3;
            angleAfter.bending = angleBefore.bending + angle.bending;
            if(angleAfter.bending < -60)
            {
                angleAfter.bending = -60;
            }
            else if(angleAfter.bending > 60)
            {
                angleAfter.bending = 60;
            }
            motor.bending = 5.432*angleAfter.bending + 2415;
        }
    }
    angleBefore = angleAfter;
}

