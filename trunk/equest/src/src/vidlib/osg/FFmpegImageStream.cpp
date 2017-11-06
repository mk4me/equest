#include "../PCH.h"
#include <vidlib/osg/FFmpegImageStream.h>
#include <vidlib/VidLibPrivate.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

int FFmpegImageStream::lockManager( void **mutex, FFmpegVideoStream::LockOp op )
{
    // pobieramy mutexa
    OpenThreads::Mutex **m=reinterpret_cast<OpenThreads::Mutex**>(mutex);
    // operacja na mutexie
    switch (op) {
        case FFmpegVideoStream::LockOpCreate:
            *m=new OpenThreads::Mutex;
            return !*m;
        case FFmpegVideoStream::LockOpDestroy:
            delete *m;
            return 0;
        case FFmpegVideoStream::LockOpObtain:
            (*m)->lock();
            return 0;
        case FFmpegVideoStream::LockOpRelease:
            (*m)->unlock();
            return 0;
        default:
            return -1;
    }
}

void FFmpegImageStream::logCallback( FFmpegVideoStream::LogSeverity severity, const char* msg, FFmpegVideoStream::FFmpegClass* item, FFmpegVideoStream::FFmpegClass* parent )
{
    std::ostringstream buff;
    if ( parent ) {
        UTILS_ASSERT(item && parent->name && item->name);
        buff << "[" << parent->name << ":" << item->name << "]";
    } else if ( item ) {
        UTILS_ASSERT(item->name);
        buff << "[" << item->name << "]";
    }
    buff << msg;

    switch ( severity ) {
        case FFmpegVideoStream::LogSeverityQuiet:
            break;

        case FFmpegVideoStream::LogSeverityPanic:
        case FFmpegVideoStream::LogSeverityFatal:
        case FFmpegVideoStream::LogSeverityError:
            OSG_FATAL<<buff.str()<<std::endl;
            break;

        case FFmpegVideoStream::LogSeverityWarning:
            OSG_WARN<<buff.str()<<std::endl;
            break;

        case FFmpegVideoStream::LogSeverityInfo:
            OSG_NOTICE<<buff.str()<<std::endl;
            break;

        case FFmpegVideoStream::LogSeverityVerbose:
            OSG_INFO<<buff.str()<<std::endl;
            break;

        case FFmpegVideoStream::LogSeverityDebug:
            OSG_DEBUG<<buff.str()<<std::endl;
            break;

        default:
            UTILS_FAIL("Nieznany poziom FFmpeg. Wiadomość: %s", buff.str());
            break;
    }
}


////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
