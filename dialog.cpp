#include "dialog.h"
#include "ui_dialog.h"
#include <QPainter>


Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Dialog),
      image1(0L),
      automaticNavigation(true),
      angle(0,0,0),
      angleBefore(0,0,0),
      angleAfter(0,0,0),
      motor(0,0,0),
      serial(new QSerialPort(this)),
      label1(new QLabel(this)),
      cThread1(new QThread(this)),
      timer(new QTimer(this)),
      grabSize(1920,1080)
{
    ui->setupUi(this);
    Epiphan1 = new Epiphan2USB(51011, grabSize, (V2U_GRABFRAME_FORMAT_RGB24 | V2U_GRABFRAME_SCALE_HW));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readMotorData()));
    //label1Geometry = QRect(0, 0, QApplication::desktop()->geometry().width(), QApplication::desktop()->geometry().height());
    //label2Geometry = QRect(0, 0, QApplication::desktop()->geometry().width()/4, QApplication::desktop()->geometry().height()/4);
    //label2 = new QLabel(this);
    label1->setGeometry(this->geometry());
    //label2->setGeometry(label2Geometry);
    label1->setStyleSheet("background-color: black");
    //label2->setStyleSheet("background-color: black");
    //cThread2 = new QThread(this);
    //Epiphan2 = new Epiphan2USB(53208);                  //Initialize Framegrabber2

    Epiphan1->InitializeThread(cThread1);
    Epiphan1->moveToThread(cThread1);
    //Epiphan2->InitializeThread(cThread2);
    //Epiphan2->moveToThread(cThread2);

    ///////////////////////////////////////////////////////////////////////////////////////////
    ////! Initialize Timer and set Buttons disabled !//////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    //timer2 = new QTimer(this);
    //timer2->setInterval(100);
    //timer2->stop();
    //connect(timer2, SIGNAL(timeout()), this, SLOT(processFrameAndSendData()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    timer->stop();
}

Dialog::~Dialog()
{
    delete ui;
}


///////////////////////////////////////////////////////////////////////////////////////////
////! Process Frame and Display  !/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void Dialog::updateFrame() {
    ////! Check if Framegrab worked  !/////////////////////////////////////////////////////////
    if(Epiphan1->framegrabIsFailed()/* || Epiphan2->framegrabIsFailed()*/) {
        QApplication::beep();
        QMessageBox::warning(this, "Warning", "USB or Videosource not connected!");
        timer->stop();
        Epiphan1->Close();
        //Epiphan2->Close();
        return;
    }

    cv::Mat resizeMat1(cv::Size(this->size().width(),this->size().height()), CV_8UC3);
    cv::resize(*image1, resizeMat1, cv::Size(this->size().width(),this->size().height()), 0, 0, CV_INTER_AREA);
    ////! Displaying Images on GUI  !//////////////////////////////////////////////////////////
    qimage1 = QImage((uchar*)resizeMat1.data, resizeMat1.cols, resizeMat1.rows, resizeMat1.step, QImage::Format_RGB888);
    if (massCenters.size() > 1) {
//        QPainter painter(&qimage1);
//        painter.setPen(QColor(255,0,0,200));
//        painter.setFont(QFont("Times", 20, QFont::Bold));
//        for (int i = 0; massCenters.size(); i++) {
//            painter.drawText(massCenters[0].x, massCenters[0].y, QString::number(i+1));
//        }
        //QTimer::singleShot(5000, this, SLOT(calcRotation()));
        //geht nicht so einfach -> copy von massCenter anlegen und vergleichen nach 5sec
        // -> immer noch gleich? -> calcRotation(), else return;a


        // Zahlenoverlay auf das qimage1
        // wenn Zahl gedrückt -> härterer Sprung! -> d.h. Rot und Bend direkt hintereinander mit rotation/5 und k = k/3
        // automaticNavigation wieder auf true und QTimer::singleshot(100, calcRot)
    }
    label1->setPixmap(QPixmap::fromImage(qimage1));
    //label2->setPixmap(QPixmap::fromImage(qimage2));
}


void Dialog::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (key == Qt::Key_Escape) {
        closeApp();
    }
    if (key == Qt::Key_Space) {
        if (!(Epiphan1->isOpen()/* && Epiphan2->isOpen()*/)) {
            if (openSerialport()) {
                initMotor();
                automaticNavigation = true;
            }
            openFrmGrab();
            startOrPause();
        }
        else {
            startOrPause();
        }
    }
    if (key == Qt::Key_1 && massCenters.size() > 1 && automaticNavigation) {
        image1_gray.getRotationAndBending(massCenters[0], angleBefore, angleAfter, motor);
        QByteArray rotationValue = QByteArray("m1") + QByteArray::number(angleAfter.rotation);
        QByteArray bendingValue = QByteArray("m2") + QByteArray::number(angleAfter.bending);
        serial->write(rotationValue);
        serial->write(bendingValue);
        QTimer::singleShot(150, this, SLOT(calcRotation()));
    }
    if (key == Qt::Key_2 && massCenters.size() > 1 && automaticNavigation) {
        image1_gray.getRotationAndBending(massCenters[1], angleBefore, angleAfter, motor);
        QByteArray rotationValue = QByteArray("m1") + QByteArray::number(angleAfter.rotation);
        QByteArray bendingValue = QByteArray("m2") + QByteArray::number(angleAfter.bending);
        serial->write(rotationValue);
        serial->write(bendingValue);
        QTimer::singleShot(150, this, SLOT(calcRotation()));
    }
    if (key == Qt::Key_3 && massCenters.size() > 2 && automaticNavigation) {
        image1_gray.getRotationAndBending(massCenters[2], angleBefore, angleAfter, motor);
        QByteArray rotationValue = QByteArray("m1") + QByteArray::number(angleAfter.rotation);
        QByteArray bendingValue = QByteArray("m2") + QByteArray::number(angleAfter.bending);
        serial->write(rotationValue);
        serial->write(bendingValue);
        QTimer::singleShot(150, this, SLOT(calcRotation()));
    }
    //    if (key == Qt::Key_Tab) {
    //        if (label1->width() < label2->width()) {
    //            label1Geometry.setRect(0, 0, QApplication::desktop()->geometry().width(), QApplication::desktop()->geometry().height());
    //            label1->setGeometry(label1Geometry);

    //            label2Geometry.setRect(0, 0, QApplication::desktop()->geometry().width()/4, QApplication::desktop()->geometry().height()/4);
    //            label2->setGeometry(label2Geometry);

    //            label1->lower();
    //            label2->raise();
    //        }
    //        else {
    //            label2Geometry.setRect(0, 0, QApplication::desktop()->geometry().width(), QApplication::desktop()->geometry().height());
    //            label2->setGeometry(label2Geometry);

    //            label1Geometry.setRect(0, 0, QApplication::desktop()->geometry().width()/4, QApplication::desktop()->geometry().height()/4);
    //            label1->setGeometry(label1Geometry);

    //            label2->lower();
    //            label1->raise();
    //        }
    //    }

    if (key == Qt::Key_S) {
        saveCurrentImage();
    }
    if (key == Qt::Key_C) {
        if (serial->isOpen())
            serial->close();
        else {
            openSerialport();
        }
    }
}

void Dialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    QApplication::quit();
}


void Dialog::closeApp() {
    timer->stop();
    Epiphan1->stopDoWork();
    //Epiphan2->stopDoWork();
    //connect(cThread2, SIGNAL(finished()), this, SLOT(close()));
    connect(cThread1, SIGNAL(finished()), this, SLOT(close()));     //bei 2 Kameras cThread2 verwenden, cThread1 löschen
    //if (!cThread2->isRunning())
    if (!cThread1->isRunning())     //bei 2 Kameras cThread2 verwenden, cThread1 löschen
        close();
    return;
}

void Dialog::openFrmGrab() {
    if(Epiphan1->Open()/* && Epiphan2->Open()*/) {
        image1 = new cv::Mat(grabSize.height(), grabSize.width(), CV_8UC3, Epiphan1->getImageData());
        //        image2 = new cv::Mat(720, 1280, CV_8UC3, Epiphan2->getImageData());
    }
    else {
        QApplication::beep();
        QMessageBox::warning(this, "Information", "Framegrabber not properly connected!");
    }
    return;
}

void Dialog::startOrPause() {
    if(timer->isActive()) {
        timer->stop();
        Epiphan1->stopDoWork();
        //Epiphan2->stopDoWork();
    }
    else if(!timer->isActive() && Epiphan1->isOpen() /*&& Epiphan2->isOpen()*/) {
        Epiphan1->startDoWork();
        //Epiphan2->startDoWork();
        while(cThread1->isFinished()/* || cThread2->isFinished()*/) {}
        timer->start();
    }
    return;
}

void Dialog::saveCurrentImage() {
    time_t t;
    struct tm *now;
    std::string DateAndTime;
    std::stringstream convert;
    t = time(0);
    now = localtime( & t );
    convert     << (now->tm_year + 1900)    << '.'
                << (now->tm_mon + 1)        << '.'
                <<  now->tm_mday            << '-'
                 <<  now->tm_hour            << '.'
                  <<  now->tm_min             << '.'
                   <<  now->tm_sec             << '_';
    DateAndTime = convert.str();

    cv::imwrite("C:/Users/Lenovo/Documents/Qt-Projekte/Visual_Control_FullScreen/Images/" + DateAndTime + ".bmp", *image1);
    //cv::imwrite("C:/Users/Lenovo/Documents/Qt-Projekte/Visual_Control_FullScreen/Images/" + DateAndTime + "nose.bmp", image2);

    return;
}


bool Dialog::openSerialport(){
    if (!serial->isOpen()) {
        foreach (QSerialPortInfo info, QSerialPortInfo().availablePorts()) {
            if (info.description() == "USB Serial Port") {
                serial->setPort(info);
                if (serial->open(QIODevice::ReadWrite)) {
                    if (!(serial->setBaudRate(QSerialPort::Baud115200)
                          && serial->setDataBits(QSerialPort::Data8)
                          && serial->setParity(QSerialPort::NoParity)
                          && serial->setStopBits(QSerialPort::OneStop)
                          && serial->setFlowControl(QSerialPort::NoFlowControl))) {
                        serial->close();
                        return false;
                    }
                }
                else {
                    QApplication::beep();
                    QMessageBox::warning(this, "Error Serialport", "Serialport could not be opened!");
                    return false;
                }
                return true;
            }
        }
        return false;
    }
    return true;
}

void Dialog::calcRotation()
{
    if (automaticNavigation) {
        cv::cvtColor(*image1, image1_gray, CV_BGR2GRAY);
        cv::GaussianBlur(image1_gray, image1_gray, cv::Size(3,3), 0, 0);
        image1_gray.removeBlackFrame();
        if (image1_gray.getROI(massCenters)) {
            image1_gray.getRotation(massCenters[0], angleBefore, angleAfter, motor);
            if (serial->isOpen()) {
                QByteArray rotationValue = QByteArray("m1") + QByteArray::number(static_cast<int>(motor.rotation));
                serial->write(rotationValue);
            }
            else
                QMessageBox::warning(this, "Warning", "Serialport not open");
        }
        QTimer::singleShot(150, this, SLOT(calcBending()));
    }
}

void Dialog::calcBending()
{
    cv::cvtColor(*image1, image1_gray, CV_BGR2GRAY);
    cv::GaussianBlur(image1_gray, image1_gray, cv::Size(3,3), 0, 0);
    image1_gray.removeBlackFrame();
    if (image1_gray.getROI(massCenters)) {
        if (massCenters.size() < 10) {
            image1_gray.getBending(massCenters[0], angleBefore, angleAfter, motor);
            if (serial->isOpen()) {
                QByteArray bendingValue = QByteArray("m2") + QByteArray::number(static_cast<int>(motor.bending));
                serial->write(bendingValue);
            }
            else
                QMessageBox::warning(this, "Warning", "Serialport not open");
        }
        else {
            return;
        }
    }
    QTimer::singleShot(150, this, SLOT(calcRotation()));

    //    wenn im calcBending mehr als ein MassCenter vorhanden ist
    //            -> kein singlshot aussenden -> autoSteuerung abgebrochen
    //            -> stattdessen vllt im updateFrame() die MassCenters anhand von Zahlen ins qimage1 einfügen
}


void Dialog::readMotorData() {
    QByteArray data = serial->readLine();
    dataFromSerial = data.split('\t');
    if (data.contains("off"))
        automaticNavigation = false;
    //qDebug() << dataFromSerial;
}


void Dialog::initMotor() {
    serial->write("m12700");
    serial->write("m22470");
    QTimer::singleShot(1000, this, SLOT(goToInitialPosition()));
}


void Dialog::goToInitialPosition()
{
    serial->write("m12810");
    serial->write("m22384");
    if (Epiphan1->isOpen()) {
        QTimer::singleShot(200, this, SLOT(calcRotation()));
    }
}
