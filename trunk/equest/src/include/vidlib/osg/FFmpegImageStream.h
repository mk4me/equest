/********************************************************************
	created:  2011/01/27
	created:  27:1:2011   10:35
	filename: FFmpegImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__FFMPEGIMAGESTREAM_H__
#define HEADER_GUARD_VIDLIB__FFMPEGIMAGESTREAM_H__

#include <vidlib/osg/GenericImageStream.h>
#include <vidlib/FFmpegVideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class VIDLIB_EXPORT FFmpegImageStream : public GenericImageStream<FFmpegVideoStream>
{
public:
    /** Domyślne implementacje */
    META_Object(vidlib, FFmpegImageStream);

    /** Konstruktor zerujący */
    FFmpegImageStream()
    {}
    /** Konstruktor kopiujący */
    FFmpegImageStream(const FFmpegImageStream& image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY) :
    GenericImageStream(image, copyop)
    {}

public:
    //! Lock manager który wykorzystuje muteksy z OpenThreads.
    //! \see FFmpegVideoStream::setLockManager
    static const int lockManager(void **mutex, FFmpegVideoStream::LockOp op);
    //! Callback który przekierowywuje logi z ffmpeg do strumieni OSG. 
    //! \see FFmpegVideoStream::setLogCallback
    static void logCallback(FFmpegVideoStream::LogSeverity severity, const char* msg, FFmpegVideoStream::FFmpegClass* item, FFmpegVideoStream::FFmpegClass* parent);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__FFMPEGIMAGESTREAM_H__
