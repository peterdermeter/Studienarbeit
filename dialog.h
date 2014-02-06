#ifndef DIALOG_H
#define DIALOG_H

#include "Epiphan2USB.h"
#include "histogram.h"
#include "imageFunctions.h"


//! Qt Libraries
#include <QtWidgets>
#include <QtCore>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui
{
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

public slots:
    
private slots:
    void openFrmGrab();
    void startOrPause();
    void closeApp();
    void updateFrame();
    void saveCurrentImage();

    bool openSerialport();
    void calcRotation();
    void calcBending();
    void readMotorData();
    void goToInitialPosition();
    void initMotor();
    void checkForKeyPressed();

private:
    Ui::Dialog *ui;

    ///////////////////////////////////////////////////
    ///!  Framegrabber  !//////////////////////////////
    Epiphan2USB *Epiphan1;
    //Epiphan2USB *Epiphan2;
    QThread *cThread1;
    //QThread *cThread2;

    ///////////////////////////////////////////////////
    ///!  ImageFunctions  !////////////////////////////
    QList<cv::Point> massCenters;
    QList<cv::Point> massCentersBefore;
    EndoscopeData angle;
    EndoscopeData angleBefore;
    EndoscopeData angleAfter;
    EndoscopeData motor;
    bool automaticNavigation;


    ///////////////////////////////////////////////////
    ///!  Matrixes  !//////////////////////////////////
    cv::Mat *image1;
    Matrix image1_gray;
    //cv::Mat image2;

    ///////////////////////////////////////////////////
    ///!  QImages  !///////////////////////////////////
    QImage qimage1;
    //QImage qimage2;
    QLabel *label1;
    //QLabel *label2;
    QRect label1Geometry;
    //QRect label2Geometry;
    QSize grabSize;

    ///////////////////////////////////////////////////
    ///!  Timer/Serialport  !//////////////////////////////////////
    QTimer *timer;
    QTimer *timer2;
    QSerialPort *serial;
    QList<QByteArray> dataFromSerial;
};


#endif // DIALOG_H
