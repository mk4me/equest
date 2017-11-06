#include "../PCH.h"
#include <vidlib/osg/VideoImageStream.h>

#include <thread>
#include <iostream>

UTILS_PUSH_WARNINGS
#include <osg/CopyOp>
#include <osg/Notify>
UTILS_POP_WARNINGS

#include <utils/Align.h>
#include <vidlib/osg/VideoImageStream.h>
#include "../VidLibPrivate.h"
#include <vidlib/FFmpegVideoStream.h>

#undef min
#undef max

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

VideoImageStream::VideoImageStream() :
innerStream(NULL), 
mutex( new Mutex() ),
timer( new Timer() ),
timeScale(1.0), 
prevTimestamp(INVALID_TIMESTAMP), 
maxWidth(-1),
copies( new Copies() ),
targetFormat(defaultFormat)
{
    VIDLIB_FUNCTION_PROLOG;
    // TODO: zmienić

    utils::zero(currentPicture);
    currentPicture.format = targetFormat;
    setOrigin(osg::Image::TOP_LEFT);

    copies->get().insert(this);
}

VideoImageStream::VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop) :
osg::ImageStream(image, copyop),
timeScale(1.0),
prevTimestamp(image.prevTimestamp), 
maxWidth(image.maxWidth),
targetFormat(image.targetFormat)
{
    VIDLIB_FUNCTION_PROLOG;
    innerStream = dynamic_cast<Stream*>(copyop(image.innerStream));
    timer = dynamic_cast<Timer*>(copyop(image.timer));
    mutex = dynamic_cast<Mutex*>(copyop(image.mutex));
    copies = dynamic_cast<Copies*>(copyop(image.copies));
    
    utils::zero(currentPicture);
    currentPicture.format = targetFormat;

    copies->get().insert(this);
}

VideoImageStream::~VideoImageStream()
{
    VIDLIB_FUNCTION_PROLOG;
    OSG_DEBUG<<"Destructing VideoImageStream..."<<std::endl;

    // quit
    quit();

    OSG_DEBUG<<"Have done quit"<<std::endl;

    // release athe audio streams to make sure that the decoder doesn't retain any external
    // refences.
    getAudioStreams().clear();

    //utils::deletePtr(innerStream);

    OSG_DEBUG<<"Destructed VideoImageStream."<<std::endl;
}

//------------------------------------------------------------------------------

void VideoImageStream::run()
{
    try
    {
        // początek
        {
            ScopedLock lock(getMutex());
            // synchronizacja timera
            setStreamTime(getStream()->getTime());
        }

        bool done = false;
        osg::Timer frameLength;

        while ( _status == PLAYING && !done ) {
            frameLength.setStartTick();
            {
                // aktualizacja
                ScopedLock lock(getMutex());

                // bieżące tyknięcie
                osg::Timer& timer = getTimer();
                osg::Timer_t tick = timer.tick();
                double delta = timer.delta_s(timer.getStartTick(), tick);
                // ustawiamy czas
                setStreamTime( getStream()->getTime() + delta * timeScale );
                // "poprawiamy" timer
                timer.setStartTick(tick);
                publishFrameAndNotify();

                if ( getStream()->isEndOfStream() ) {
                    done = (_loopingMode != LOOPING);
                }
            }
            // jak długo to trwało?
            double waitTime = getStream()->getFrameDuration() / timeScale - frameLength.time_s();
            if ( waitTime > 0.0 ) {
                // czekamy
                std::this_thread::sleep_for(std::chrono::microseconds( static_cast<unsigned>(waitTime * 1000000)));
            }
        }
    } catch (const std::exception & error) {
        OSG_WARN << "VideoImageStream::run : " << error.what() << std::endl;
    } catch (...) {
        OSG_WARN << "VideoImageStream::run : unhandled exception" << std::endl;
    }
    OSG_DEBUG<<"Finished VideoImageStream::run()"<<std::endl;
}

//------------------------------------------------------------------------------

void VideoImageStream::play()
{
    VIDLIB_FUNCTION_PROLOG;
	_status = PLAYING;
}

void VideoImageStream::pause()
{
    VIDLIB_FUNCTION_PROLOG;
    _status = PAUSED;
}

void VideoImageStream::rewind()
{
    VIDLIB_FUNCTION_PROLOG;
    // trzeba synchronizować
    ScopedLock lock(getMutex());
    setStreamTime(0.0);
    publishFrameAndNotify();
}

void VideoImageStream::seek(double time)
{
    VIDLIB_FUNCTION_PROLOG;
    ScopedLock lock(getMutex());
    setStreamTime(time);
    publishFrameAndNotify();
}

void VideoImageStream::quit()
{
    VIDLIB_FUNCTION_PROLOG;
    ScopedLock lock(getMutex());
    _status = PAUSED;
    // kasujemy
    innerStream = NULL;
    currentPicture.free();
}


double VideoImageStream::getCreationTime() const
{
    OSG_FATAL<<"Not implemented."<<std::endl;
    return 0.0;
}

double VideoImageStream::getLength() const
{
    return getStream()->getDuration();
}

double VideoImageStream::getReferenceTime () const
{
    return getStream()->getTime();
}

double VideoImageStream::getFrameRate() const
{
    return getStream()->getFramerate();
}

bool VideoImageStream::isImageTranslucent() const
{
    return false;
}

void VideoImageStream::applyLoopingMode()
{
    ImageStream::applyLoopingMode();
}

void VideoImageStream::setTimeMultiplier( double timescale )
{
    timeScale = timescale;
}

double VideoImageStream::getTimeMultiplier() const
{
    return timeScale;
}



void VideoImageStream::update( osg::NodeVisitor* /*nv*/ )
{  
}

bool VideoImageStream::requiresUpdateCall() const
{
    return false;
}

void VideoImageStream::setMaxWidth( int maxWidth )
{
    VIDLIB_FUNCTION_PROLOG;
    this->maxWidth = maxWidth;

    int width = 0;
    int height = 0;
    int streamWidth = getStream()->getWidth();
    int streamHeight = getStream()->getHeight();

    // jaka szerokość zostanie zastosowana?
    if ( maxWidth >= streamWidth ) {
        width = streamWidth;
        height = streamHeight;
    } else {
        int widthDiv = static_cast<int>((static_cast<double>(streamWidth)/maxWidth + 0.334));
        width = std::max(16, streamWidth / widthDiv);
        height = std::max(16, streamHeight / widthDiv);
        //if ( targetFormat == PixelFormatYV12 ) {
            width = UTILS_ALIGN(width, 2);
            height = UTILS_ALIGN(height, 2);
        //}
    }

    // czy realokacja ramki?
    if ( currentPicture.width != width ) {
        //OSG_NOTICE<<getStream()->getSource()<<":Changing frame size from "<<currentPicture.width<<" to "<<width<<std::endl;
        ScopedLock lock(getMutex());
        currentPicture.free();
        currentPicture = Picture::create(width, height, targetFormat);
        reloadImage(currentPicture);
    }
}

void VideoImageStream::setStreamTime( double time )
{
    VIDLIB_FUNCTION_PROLOG;
    getStream()->setTime(time);
    getTimer().setStartTick();
}


void VideoImageStream::publishFrameAndNotify()
{
    publishFrame();
    Copies::value_type& copies = this->copies->get();
    for ( Copies::value_type::iterator it = copies.begin(); it != copies.end(); ++it ) {
        VideoImageStream* stream = it->get();
        if ( stream && stream != this ) {
            stream->publishFrame();
        }
    }
}


void VideoImageStream::publishFrame()
{
    VIDLIB_FUNCTION_PROLOG;
    // poprzedni timestamp
    if ( prevTimestamp != getStream()->getFrameTimestamp() ) {
        prevTimestamp = getStream()->getFrameTimestamp();
        reloadImage(currentPicture);
    }
}

void VideoImageStream::reloadImage(Picture & picture)
{
    VIDLIB_FUNCTION_PROLOG;
    getStream()->getFrame(picture);
    switch (targetFormat) {

        case PixelFormatYV12:
            setImage(picture.width, picture.dataHeight,
                1, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                picture.data, NO_DELETE);
            break;
        
        case PixelFormatRGB24:
            setImage(picture.width, picture.height,
                1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
                picture.data, NO_DELETE);
            break;

        case PixelFormatBGRA:
            setImage(picture.width, picture.height,
                1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
                picture.data, NO_DELETE);
            break;

        default:
            OSG_FATAL<<"Unknown image format."<<std::endl;
            setImage(picture.width, picture.dataHeight,
                1, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                picture.data, NO_DELETE);
            break;

    }
}

void VideoImageStream::setTargetFormat( VIDLIB_PixelFormat targetFormat )
{
    if ( targetFormat != this->targetFormat ) {

        if ( targetFormat == PixelFormatYV12 ) {
            // ponieważ wymagamy konwersji w shaderach, ustawiamy aspect ratio
            setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()) * 1.5f );
        } else {
            setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()));
        }

        this->targetFormat = targetFormat;
        int width = currentPicture.width;
        int height = currentPicture.height;
        currentPicture.free();
        currentPicture.format = targetFormat;
        currentPicture = Picture::create(width, height, targetFormat);
        reloadImage(currentPicture);
    }
}

void VideoImageStream::setStream( VideoStream* stream )
{
    // ustawienie strumienia
    innerStream = new Stream(stream);

    setFileName(stream->getSource());

    // ustawiamy zerowy czas
    setStreamTime(0.0);
    setMaxWidth(getStream()->getWidth());
    prevTimestamp = getStream()->getFrameTimestamp();

    // aspect ratio
    if ( targetFormat == PixelFormatYV12 ) {
        // poniewaz wymagamy konwersji w shaderach, ustawiamy aspect ratio
        setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()) * 1.5f );
    } else {
        setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()));
    }

    // info
    OSG_DEBUG<<"open("<<stream->getSource()<<") size("<<s()<<", "<<t()<<") aspect ratio "<<getPixelAspectRatio()<<std::endl;
    OSG_DEBUG<<"\tinner format: "<<utils::Enum<VIDLIB_PixelFormat>::getName(getStream()->getPixelFormat())<<std::endl;
    OSG_DEBUG<<"\tstream format: "<<utils::Enum<VIDLIB_PixelFormat>::getName(targetFormat)<<std::endl;

    // domylnie pauzujemy
    _status = PAUSED;
}


////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
