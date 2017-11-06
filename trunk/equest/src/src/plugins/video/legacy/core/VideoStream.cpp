#include "../VideoPCH.h"
#include <math.h>
#include <utils/Debug.h>
#include <utils/Profiler.h>
#include <utils/Enum.h>
#include <plugins/video/core/VideoStream.h>
#include <plugins/video/core/Converter.h>
#include "VMPrivate.h"
#include "FFmpegVideoStream.h"
#include <plugins/video/core/BufferedVideoStream.h>


////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

VideoStream::VideoStream( const std::string& source )
{
    VM_FUNCTION_PROLOG;
    this->source = source;
    lastError = NULL;
    frameRate = 0.0;
    frameDuration = 0.0;
    duration = 0.0;
    format = PixelFormatYV12;
    width = 0;
    height = 0;
    aspectRatio = 0.0;
    converter = new Converter();
    callback = NULL;
}


VideoStream::~VideoStream()
{
    VM_FUNCTION_PROLOG;
    utils::deletePtr(lastError);
    utils::deletePtr(converter);
}


VideoStream* VideoStream::create( Type type, const std::string& path, int bufferSize /*= 0*/ )
{
    // tworzenie odpowiedniego strumienia
    VideoStream* result = NULL;
    if ( type & FFmpeg ) {
        result = new FFmpegVideoStream(path);
    } else {
#ifdef VM_ENABLE_EXCEPTIONS
        throw new std::runtime_error("Not implemented.");
#else
        return NULL;
#endif
    }
    // czy strumień ma być buforowany?
    if ( bufferSize ) {
        return new BufferedVideoStream(result, bufferSize);
    } else {
        return result;
    }
}


bool VideoStream::notifyError( const VideoError& error )
{
    VM_FUNCTION_PROLOG;
#ifdef VM_ENABLE_EXCEPTIONS
    throw error;
#else   // VM_ENABLE_EXCEPTIONS
    utils::deletePtr(lastError);
    lastError = error.clone();
#endif  // VM_ENABLE_EXCEPTIONS
    return false;
}


bool VideoStream::onAfterInit( const std::string& source, double frameRate, double duration, PixelFormat format, int width, int height, double aspectRatio )
{
    VM_FUNCTION_PROLOG;
    this->frameRate = frameRate;
    this->frameDuration = 1.0 / frameRate;
    this->duration = duration;
    this->format = format;
    this->width = width;
    this->height = height;
    this->aspectRatio = aspectRatio;
    return true;
}

const VideoError * VideoStream::getLastError() const
{
    VM_FUNCTION_PROLOG;
#ifdef VM_ENABLE_EXCEPTIONS
    UTILS_ASSERT(false, "Should not be here...");
    return NULL;
#else   // AVENABLE_EXCEPTIONS
    return lastError;
#endif  // AVENABLE_EXCEPTIONS
}


// bool VideoStream::getFrame( FrameData & dst )
// {
//   // najpierw próbujemy pobrać bieżącą ramkę
//   FrameData src;
//   if ( getCurrentFrameData(src) ) {
//     return converter->convert( src, dst );
//   } else {
//     // jeżeli strumień nie jest w stanie upakować danych do ramki próbujemy
//     // skonwertować obie ramki do postaci zdjęcia
//     FrameLayered srcPicture, dstPicture;
//     if ( getCurrentFrameData(srcPicture) && dst.translate(&dstPicture) ) {
//       return converter->convert(srcPicture, dstPicture);
//     } else {
//       VM_ERROR( VideoError("Conversion not possible.") );
//     }
//   }
// }

bool VideoStream::getFrame( Picture & dst )
{
    VM_FUNCTION_PROLOG;
    Picture src;
    if ( getData(src) ) {
        return converter->convert( src, dst );
    } else {
        // jeżeli strumień nie jest w stanie upakować danych do ramki próbujemy
        // skonwertować obie ramki do postaci zdjęcia
        PictureLayered layers = dst.getLayers();
        return getFrame(layers);
    }
}

bool VideoStream::getFrame( PictureLayered & dst )
{
    VM_FUNCTION_PROLOG;
    // jeżeli strumień nie jest w stanie upakować danych do ramki próbujemy
    // skonwertować obie ramki do postaci zdjęcia
    PictureLayered srcPicture;
    if ( getData(srcPicture) ) {
        return converter->convert(srcPicture, dst);
    } else {
        VM_ERROR( VideoError("Conversion not possible.") );
    }
}

void VideoStream::onFrameRead()
{
    VM_FUNCTION_PROLOG;
    if ( callback ) {
        Picture picture;
        Picture * passedPicture = getData(picture) ? &picture : NULL;
        PictureLayered layers;
        PictureLayered * passedLayers = getData(layers) ? &layers : NULL;
        callback->frameRead(passedPicture, passedLayers, getFrameTimestamp(), getNextFrameTimestamp());
    }
}

int VideoStream::getFrameCount() const
{
    VM_FUNCTION_PROLOG;
    return static_cast<int>(getDuration() / getFrameDuration() + 0.5);
}

double VideoStream::getNormalizedTime() const
{
    double result = getTime() / getDuration();
    UTILS_ASSERT(result >= 0.0 && result <= 1.0);
    return result;
}

void VideoStream::setNormalizedTime( double normalizedTime )
{
    UTILS_ASSERT(normalizedTime >= 0.0 && normalizedTime <= 1.0);
    setTime( normalizedTime * getDuration() );
}

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////
