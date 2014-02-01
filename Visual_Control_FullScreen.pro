#-------------------------------------------------
#
# Project created by QtCreator 2013-11-26T14:42:32
#
#-------------------------------------------------
QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Visual_Control_FullScreen
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    Epiphan2USB.cpp \
    histogram.cpp \
    imageFunctions.cpp

HEADERS  += dialog.h \
    Epiphan2USB.h \
    histogram.h \
    imageFunctions.h

FORMS    += dialog.ui

DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += C:/OpenCV-2.4.8/include

CONFIG(release, debug|release){
    LIBS += -LC:/OpenCV-2.4.8/x86/vc10/lib \
    -lopencv_calib3d248 \
    -lopencv_contrib248 \
    -lopencv_core248 \
    -lopencv_features2d248 \
    -lopencv_flann248 \
    -lopencv_gpu248 \
    -lopencv_highgui248 \
    -lopencv_imgproc248 \
    -lopencv_legacy248 \
    -lopencv_ml248 \
    -lopencv_nonfree248 \
    -lopencv_objdetect248 \
    -lopencv_ocl248 \
    -lopencv_photo248 \
    -lopencv_stitching248 \
    -lopencv_superres248 \
    -lopencv_ts248 \
    -lopencv_video248 \
    -lopencv_videostab248 \
}
CONFIG(debug, debug|release){
    LIBS += -LC:/OpenCV-2.4.8/x86/vc10/lib \
    -lopencv_calib3d248d \
    -lopencv_contrib248d \
    -lopencv_core248d \
    -lopencv_features2d248d \
    -lopencv_flann248d \
    -lopencv_gpu248d \
    -lopencv_highgui248d \
    -lopencv_imgproc248d \
    -lopencv_legacy248d \
    -lopencv_ml248d \
    -lopencv_nonfree248d \
    -lopencv_objdetect248d \
    -lopencv_ocl248d \
    -lopencv_photo248d \
    -lopencv_stitching248d \
    -lopencv_superres248d \
    -lopencv_ts248d \
    -lopencv_video248d \
    -lopencv_videostab248d \
}


win32: LIBS += -L$$PWD/../../epiphan_sdk-3.28.0.0007/epiphan/frmgrab/lib/win/Win32/ -lfrmgrab

INCLUDEPATH += $$PWD/../../epiphan_sdk-3.28.0.0007/epiphan/frmgrab/include
DEPENDPATH += $$PWD/../../epiphan_sdk-3.28.0.0007/epiphan/frmgrab/include
