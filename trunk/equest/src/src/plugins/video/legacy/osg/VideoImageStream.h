#ifndef HEADER_GUARD_OSGFFMPEG_VIDEO_IMAGE_STREAM_H
#define HEADER_GUARD_OSGFFMPEG_VIDEO_IMAGE_STREAM_H

#include <set>
#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include <osg/ImageStream>
#include <OpenThreads/Thread>
#include <osg/Timer>


#include <utils/PtrWrapper.h>
#include <utils/PtrPolicyOSG.h>

#include "../core/VM.h"

////////////////////////////////////////////////////////////////////////////////
namespace video {
namespace osgPlugin {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Strumień wideo.
 */
class VideoImageStream : public osg::ImageStream, public OpenThreads::Thread
{
public:
    //! Domyślny format.
    static const PixelFormat defaultFormat = PixelFormatBGRA;

protected:
    //! Wrapper na surowy strumień.
    typedef utils::PtrWrapper<VideoStream, utils::PtrPolicyOSG> Stream;
    //! Wrapper na mutex.
    typedef utils::Adapter<OpenThreads::Mutex, utils::PtrPolicyOSG> Mutex;
    //! Wrapper na timer.
    typedef utils::Adapter<osg::Timer, utils::PtrPolicyOSG> Timer;
    //! Typ blokady opartej na muteksie.
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
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
    PixelFormat targetFormat;
    //! Maksymalna szerokość.
    int maxWidth;

    /** Bufor na bieżącą ramkę */
    // FrameData frameData;
    Picture currentPicture;

public:
    /** Konstruktor zerujący */
    VideoImageStream();
    /** Konstruktor kopiujący */
    VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
    /** Domyślne implementacje */
    META_Object(osgPlugin, VideoImageStream);

    /** Odtwarzanie strumienia (nowy wątek!) */
    virtual void play();
    /** Zatrzymanie strumienia */
    virtual void pause();
    /** Odtwarzanie "do tyłu" */
    virtual void rewind();
    /** Wyszukiwanie w strumieniu */
    virtual void seek(double time);
    /** Zamknięcie strumienia */
    virtual void quit(bool waitForThreadToExit = true);

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

    /** Otwiera strumień */
    virtual bool open(const std::string & filename);

    //!
    virtual bool requiresUpdateCall() const;
    //!
    virtual void update(osg::NodeVisitor* nv);

    //! \return Format do którego film jest konwertowany.
    inline PixelFormat getTargetFormat() const
    {
        return targetFormat;
    }
    //! \param targetFormat Format do którego film jest konwertowany.
    void setTargetFormat(PixelFormat targetFormat); 

    //! \return Maksymalna szerokość konwertowanego strumienia.
    inline int getMaxWidth() const
    { 
        return maxWidth;
    }
    //! \param maxWidth Maksymalna szerokość konwertowanego strumienia.
    void setMaxWidth(int maxWidth);

protected:
    /** */
    virtual ~VideoImageStream();
    /** Wątek odtwarzający */
    virtual void run();
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
    inline OpenThreads::Mutex& getMutex()
    {
        return mutex->get();
    }
    //!
    inline osg::Timer& getTimer()
    {
        return timer->get();
    }

};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgPlugin
} // namespace video
////////////////////////////////////////////////////////////////////////////////



#endif // HEADER_GUARD_OSGFFMPEG_VIDEO_IMAGE_STREAM_H
