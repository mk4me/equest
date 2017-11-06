/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   15:35
	filename: VideoImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__
#define HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__

#include <vidlib/Config.h>
#include <utils/Utils.h>

#include <mutex>
#include <set>

// POPa zrobimy na końcu, bo inaczej powstaną warningi związane z tym, że
// osg::ref_ptr nie ma DLL interface; OSG domyślnie wyłącza te warningi w
// nagłówkach, więc zdajemy się na ichnie rozwiązania
UTILS_PUSH_WARNINGS
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/ImageStream>
#include <osg/Timer>
UTILS_POP_WARNINGS

#include <utils/PtrWrapper.h>
#include <osgutils/PtrPolicyOSG.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/VideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////
UTILS_PUSH_WARNINGS
UTILS_DISABLE_DLL_INTERFACE_WARNING

/**
 *	Strumień wideo.
 */
class VIDLIB_EXPORT VideoImageStream : public osg::ImageStream
{
public:
    /** Domyślne implementacje */
    META_Object(vidlib, VideoImageStream);

    //! Domyślny format.
    static const VIDLIB_PixelFormat defaultFormat = PixelFormatBGRA;

protected:
    //! Wrapper na surowy strumień.
    typedef utils::PtrWrapper<VideoStream, utils::PtrPolicyOSG> Stream;
    //! Wrapper na mutex.
    typedef utils::Adapter<std::mutex, utils::PtrPolicyOSG> Mutex;
    //! Wrapper na timer.
    typedef utils::Adapter<osg::Timer, utils::PtrPolicyOSG> Timer;
    //! Typ blokady opartej na muteksie.
    typedef std::lock_guard<std::mutex> ScopedLock;
    //! Typ płytkich kopii strumienia.
    typedef utils::Adapter<std::set<osg::observer_ptr<VideoImageStream> >, utils::PtrPolicyOSG > Copies;

    //! Wewnętrzy strumień.
    osg::ref_ptr<Stream> innerStream; 
    /** Mutex */
    osg::ref_ptr<Mutex> mutex;
    /** Timer */
    osg::ref_ptr<Timer> timer;
    /** */
    osg::ref_ptr<Copies> copies;

    /** Skala czasowa */
    double timeScale;
    /** Poprzednia pozycja w strumieniu */
    double prevTimestamp;
    /** Format piksela */
    VIDLIB_PixelFormat targetFormat;
    //! Maksymalna szerokość.
    int maxWidth;
    /** Bufor na bieżącą ramkę */
    Picture currentPicture;

public:
    /** Konstruktor zerujący */
    VideoImageStream();
    /** Konstruktor kopiujący */
    VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
    /** */
    virtual ~VideoImageStream();

    //! Otwiera strumień
    //! \param filename Ścieżka do pliku.
    //! \param dummy Pozostawić wartość domyślną.
    template <class VideoStreamImpl>
    bool open(const std::string & filename, VideoStreamImpl* = 0)
    {
        try {
            setStream( new VideoStreamImpl(filename) );
            return true;
        } catch ( VideoError& error ) {
            OSG_FATAL<<"open("<<filename<<") reported an error: "<< error.what() << std::endl;
            return false;
        }
    }

	/** Wątek odtwarzający */
	void run();

    /** Odtwarzanie strumienia (nowy wątek!) */
    virtual void play();
    /** Zatrzymanie strumienia */
    virtual void pause();
    /** Odtwarzanie "do tyłu" */
    virtual void rewind();
    /** Wyszukiwanie w strumieniu */
    virtual void seek(double time);
    /** Zamknięcie strumienia */
    virtual void quit();

    /** Czas utworzenia */
    virtual double getCreationTime() const;
    /** Długość strumienia */
    virtual double getLength() const;
    /** Liczba klatek na sekundę */
    virtual double getFrameRate() const;
    /** Czas w strumieniu */
    virtual double getReferenceTime() const;
    /** Czas w strumieniu */
    virtual void setReferenceTime(double value) { seek(value); }
    /** Skala czasowa */
    virtual void setTimeMultiplier(double timescale);
    /** Skala czasowa */
    virtual double getTimeMultiplier() const;
    /** Czy wideo ma kanał alpha? */
    virtual bool isImageTranslucent() const;

    //!
    virtual bool requiresUpdateCall() const;
    //!
    virtual void update(osg::NodeVisitor* nv);

    //! \return Format do którego film jest konwertowany.
    inline VIDLIB_PixelFormat getTargetFormat() const
    {
        return targetFormat;
    }
    //! \param targetFormat Format do którego film jest konwertowany.
    void setTargetFormat(VIDLIB_PixelFormat targetFormat); 

    //! \return Maksymalna szerokość konwertowanego strumienia.
    inline int getMaxWidth() const
    { 
        return maxWidth;
    }
    //! \param maxWidth Maksymalna szerokość konwertowanego strumienia.
    void setMaxWidth(int maxWidth);

protected:

    /** Tryb zapętlania */
    virtual void applyLoopingMode();
    /** Ustawia bieżącą ramkę (jeśli jest nowsza niż poprzednia)*/
    virtual void publishFrameAndNotify();
    /** Ustawia bieżącą ramkę (jeśli jest nowsza niż poprzednia)*/
    virtual void publishFrame();
    /** Ustawia bieżący czas */
    virtual void setStreamTime( double time );
    /** Ustawia bieżącą ramkę */
    void reloadImage(Picture& picture);


    //! \param stream Wewnętrzny strumień.
    void setStream( VideoStream* stream );

    //! \return Wewnętrzny strumień.
    inline VideoStream* getStream()
    {
        return *(innerStream.get());
    }
    //! \return Wewnętrzny strumień.
    inline const VideoStream* getStream() const
    {
        return *(innerStream.get());
    }
    //!
    inline std::mutex& getMutex()
    {
        return mutex->get();
    }
    //!
    inline osg::Timer& getTimer()
    {
        return timer->get();
    }

};

UTILS_POP_WARNINGS
////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif // HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__
