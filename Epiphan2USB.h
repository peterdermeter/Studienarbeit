#ifndef EPIPHAN2USB_H
#define EPIPHAN2USB_H

//! C/C++ Standard Libraries
#include <iostream>
#include <string.h>
#include <ctime>


//! Epiphan DVI2USB Solo Libraries
#include <v2u_compression.h>
#include <v2u_defs.h>
#include <v2u_dshow.h>
#include <v2u_id.h>
#include <v2u_ioctl.h>
#include <v2u_version.h>
#include <frmgrab.h>

//! Qt Libraries
#include <QThread>
#include <QDebug>
#include <QSize>


//! OpenCV Libraries
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>




class Epiphan2USB : public QObject
{
    Q_OBJECT

public:
    //! Default Constructor
    explicit Epiphan2USB(const unsigned int &serialNumber, const QSize imageSize, const V2U_UINT32 format, QObject *parent = 0);

    //! Default Destructor
    ~Epiphan2USB();

    //! Initializes the video source
    bool    Open();

    //! Shuts down the video source
    bool    Close();

    //! Returns true if the video source is initialized, false otherwise
    bool    isOpen() const { return Epiphan2USB_open; }

    //!
    bool    getImageFormat(unsigned int& nWidth, unsigned int& nHeight, unsigned int& nBitCount) const;

    unsigned char* getImageData() { return ImageData; }

    bool	setCropArea(const unsigned int& x0, const unsigned int& y0, const unsigned int& nWidth, const unsigned int& nHeight);

    bool	getCropArea(unsigned int &nCropX0, unsigned int &nCropY0, unsigned int &nCropWidth, unsigned int &nCropHeight, unsigned int &nWidth, unsigned int &nHeight) const;

    unsigned int getImageDataSize() const { return ImageDataSize; }

    V2U_UINT32 getGrabFormat() const { return GrabFormat; }

    void	setGrabFormat(const V2U_UINT32 &format);

    //! Says true, if Framegrab failed
    bool framegrabIsFailed() const { return framegrab_failed; }

    void    InitializeThread(QThread *cThread);

    void    stopDoWork();

    void    startDoWork();

private:
    Epiphan2USB(const Epiphan2USB &);                          // hiding Copy-Constructor for protection
    FrmGrabber*     Grabber;                                   //Pointer to Epiphan2USB
    V2U_GrabFrame2* Frame;                                     //Pointer to grabbed Frame
    V2URect         cropRect;                                  //CropRect Structur;
    V2U_VideoMode   vm;                                        //V2U_VideoMode Structur
    V2U_UINT32      GrabFormat;

    bool            video_mode_detected;
    bool            Epiphan2USB_open;
    bool            framegrab_failed;
    bool            stopThread;
    unsigned char*  ImageData;
    unsigned int    sn;
    unsigned int	ImageWidth, ImageHeight, ImageBpp;
    unsigned int	ImageDataSize;

    
signals:
    void finished();
    void started();

private slots:
    void    DoWork();
    
};

#endif // EPIPHAN2USB_H
