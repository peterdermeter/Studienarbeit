#include "Epiphan2USB.h"


Epiphan2USB::Epiphan2USB(const unsigned int &serialNumber, const QSize imageSize, const V2U_UINT32 format, QObject *parent)
    :  QObject(parent),
      Grabber((FrmGrabber*) malloc(sizeof(FrmGrabber*))),
      Frame((V2U_GrabFrame2*) malloc(sizeof(V2U_GrabFrame2))),
      ImageData(0L),
      sn(serialNumber),
      ImageBpp(V2UPALETTE_2_BPP(format)),
      GrabFormat(format),
      ImageWidth(imageSize.width()),
      ImageHeight(imageSize.height()),
      ImageDataSize(ImageWidth*ImageHeight*ImageBpp/8),
      video_mode_detected(false),
      Epiphan2USB_open(false),
      framegrab_failed(false),
      stopThread(false)
{
    cropRect.x = 0;
    cropRect.y = 0;
    cropRect.width = ImageWidth;
    cropRect.height = ImageHeight;
    memset (Grabber, 0, sizeof(FrmGrabber*));                       //setting allocated memory to zero
    memset(Frame, 0, sizeof(V2U_GrabFrame2));                       //setting allocated memory to zero
}



Epiphan2USB::~Epiphan2USB()
{
    Close();
}

bool Epiphan2USB::Open()
{
    if(!Epiphan2USB_open)
    {
        Grabber = FrmGrabLocal_OpenSN((QString("D3U") + QString::number(sn)).toLocal8Bit());                  //Pointer is set to Epiphan2USB
        if (!Grabber) {
            return false;
        }
        Epiphan2USB_open = true;
        ImageData = new unsigned char[ImageDataSize];                       //allocating memory for Imagedata
        framegrab_failed = false;
    }
    return true;
}

bool Epiphan2USB::Close()
{
    if(Epiphan2USB_open)
    {
        FrmGrab_Close(Grabber);
        delete ImageData;                 //free memory
        ImageData = 0L;
        Epiphan2USB_open = false;
        video_mode_detected = false;
    }
    return true;
}



bool Epiphan2USB::getImageFormat(unsigned int& nWidth, unsigned int& nHeight, unsigned int& Bpp) const
{
    if (!Epiphan2USB_open) return false;

    nWidth = ImageWidth;
    nHeight = ImageHeight;
    Bpp = ImageBpp;

    return true;
}


bool Epiphan2USB::setCropArea(const unsigned int& x0, const unsigned int& y0, const unsigned int& nWidth, const unsigned int& nHeight)
{
    if (!Epiphan2USB_open) return false;

    cropRect.x = x0;
    cropRect.y = y0;
    cropRect.width = nWidth;
    cropRect.height = nHeight;
    return true;
}

bool Epiphan2USB::getCropArea(unsigned int &nCropX0, unsigned int &nCropY0, unsigned int &nCropWidth, unsigned int &nCropHeight, unsigned int &nWidth, unsigned int &nHeight) const
{
    if (!Epiphan2USB_open) return false;

    nCropX0 = cropRect.x;
    nCropY0 = cropRect.y;
    nCropWidth = cropRect.width;
    nCropHeight = cropRect.height;
    nWidth = ImageWidth;
    nHeight = ImageHeight;

    return true;
}

void Epiphan2USB::setGrabFormat(const V2U_UINT32 &format)
{
    GrabFormat = format;
    ImageBpp	= V2UPALETTE_2_BPP (GrabFormat);
    ImageDataSize = ImageWidth * ImageHeight * ImageBpp/8;
    return;
}

void Epiphan2USB::InitializeThread(QThread *cThread)
{
    connect(cThread, SIGNAL(started()), this, SLOT(DoWork()));
    connect(this, SIGNAL(finished()), cThread, SLOT(quit()));
    connect(this, SIGNAL(started()), cThread, SLOT(start()));
    return;
}

void Epiphan2USB::startDoWork()
{
    stopThread = false;
    emit started();
    return;
}

void Epiphan2USB::stopDoWork()
{
    stopThread = true;
    return;
}

void Epiphan2USB::DoWork()
{
    while(!stopThread)
    {
        if (!Epiphan2USB_open) break;
        Frame = FrmGrab_Frame(Grabber, GrabFormat, &cropRect);
        if(!Frame)
        {
            framegrab_failed = true;
            break;
        }

        memcpy(this->ImageData, (unsigned char*)Frame->pixbuf, ImageDataSize);
        FrmGrab_Release(Grabber, Frame);
    }
    emit finished();
    return;
}
