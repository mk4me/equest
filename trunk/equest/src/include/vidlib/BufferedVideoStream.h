/********************************************************************
	created:  2010/07/20
	created:  20:7:2010   16:31
	filename: BufferedVideoStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__BUFFEREDVIDEOSTREAM_H__
#define HEADER_GUARD_VIDLIB__BUFFEREDVIDEOSTREAM_H__

#include <vidlib/VideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class VideoBuffer;

class VIDLIB_EXPORT BufferedVideoStream : public VideoStream, public VideoStream::Callback
{
private:
    //! Bufor na ramki
    VideoBuffer * convertedFrames;
    //! Bufor na ramki jeszcze nie przekonwertowane.
    VideoBuffer * pendingFrames;
    //! Strumień który jest buforowany.
    VideoStream * innerStream;
    //! Maksymalny rozmiar bufora.
    int maxBufferSize;
    //! Wybrany czas (pomiędzy bieżącym faktycznym timestampem a czasem następnej ramki).
    double wantedTime;

public:
    //! 
    //! \param innerStream Strumień buforowany. Przejmowany na właśność.
    //! \param maxBufferSize Maksymalny rozmiar bufora.
    BufferedVideoStream(VideoStream * innerStream, int maxBufferSize = INT_MAX);
    //! Zwalnia bufor oraz strumień wewnętrzny.
    virtual ~BufferedVideoStream();

    //! Odczytuje następną ramkę.
    virtual bool readNext();
    //! \retrun Pozycja w strumieniu.
    virtual double getTime() const;
    //! \param time Pozycja w źródle
    virtual bool setTime(double time);
    //! \return Prawdziwy timestamp ramki.
    virtual double getFrameTimestamp() const;
    //! \return Prawdziwy timestamp ramki.
    virtual double getNextFrameTimestamp() const;

public:
    //! Callback wewnętrznego strumienia.
    void frameRead(Picture * picture, PictureLayered * layers, double timestamp, double endTimestamp);

private:
    //!
    Picture* popBuffer( double time );
    //! 
    //! \param result
    bool returnOrCopyError(bool result);
    //!
    virtual bool getData(Picture & dst);

    //const BufferedFrame * getFrame(double time) const;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__BUFFEREDVIDEOSTREAM_H__
