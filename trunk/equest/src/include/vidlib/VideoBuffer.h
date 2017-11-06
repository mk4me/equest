/********************************************************************
	created:  2010/07/28
	created:  28:7:2010   14:40
	filename: VideoBufferImpl.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOBUFFER_H__
#define HEADER_GUARD_VIDLIB__VIDEOBUFFER_H__

#include <list>
#include <vector>
#include <vidlib/Config.h>


////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class VideoBufferChunk;

//! Bufor ramek o maksymalnym zadanym rozmiarze.
class VIDLIB_EXPORT VideoBuffer
{
private:
    //! Zastosowany wzorzec "Pimpl" (żeby usunąć warningi przy budowie DLL)
    class VideoBufferImpl* impl;

public:
    //! \param maxSize
    //! \param width
    //! \param height
    //! \param format
    VideoBuffer(int maxSize, int width, int height, VIDLIB_PixelFormat format);
    //! Destruktor, zwalnia cały bufor.
    ~VideoBuffer();

    //! \return
    VIDLIB_PixelFormat getFormat() const;

    //! \return Wskaźnik na bufor ramki albo NULL, jeżeli wyczerpało się miejsce w buforze.
    Picture * pop();
    //! \param frame Ramka, która ma być zwrócona do bufora. Wywoływać TYLKO, jeśli po użyciu
    //!              metody pop ramka nie została użyta.
    void push(Picture * frame);

    //! Zaznacza, że ramka jest użyta.
    //! \param frame
    //! \param startTime Początek przedziału czasu.
    //! \param endTime Koniec przedziału czasu.
    bool notifyUsed(Picture * frame, double startTime, double endTime);

    //! Zwalania ramkę w zadanym punkcie czasowym.
    bool freeFrame(double time);
    //!
    bool freeFirstFrame();
    //!
    bool freeLastFrame();

    //! \param timestamp
    //! \param finish
    const Picture * getLastFrame(double * timestamp = NULL, double * finish = NULL);
    //! \param timestamp
    //! \param finish
    const Picture * getFirstFrame(double * timestamp = NULL, double * finish = NULL);

    //! \param time
    //! \param timestamp
    //! \param finish
    //! \return Zbuforowana ramka dla zdanego czasu bądź NULL, jeżeli nie jest zbuforowana.
    const Picture * getFrame(double time, double * timestamp = NULL, double * finish = NULL);
    //! \param time
    //! \param timestamp
    //! \param finish
    //! \return Zbuforowana ramka dla zdanego czasu bądź NULL, jeżeli nie jest zbuforowana.
    const Picture * getNearestFrame(double time, double * timestamp = NULL, double * finish = NULL);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD_VIDLIB__VIDEOBUFFER_H__
