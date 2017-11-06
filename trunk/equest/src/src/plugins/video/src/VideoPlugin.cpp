#include "VideoPCH.h"
#include <corelib/IPlugin.h>
#include "VideoParser.h"
#include <vidlib/osg/FFmpegImageStream.h>
#include "VideoVisualizer.h"

CORE_PLUGIN_BEGIN("video", core::UID::GenerateUniqueID("{4F39348A-3364-4B2F-B748-B1CF578C0628}"));
    CORE_PLUGIN_ADD_PARSER(VideoParser);
    CORE_PLUGIN_ADD_VISUALIZER(VideoVisualizer);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(::VideoStream);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(vidlib::Picture);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(VideoChannel);
CORE_PLUGIN_END;


//! \param severity
//! \param msg
//! \param item
//! \param parent
void logCallback(vidlib::FFmpegVideoStream::LogSeverity severity, const char* msg, vidlib::FFmpegVideoStream::FFmpegClass* item, vidlib::FFmpegVideoStream::FFmpegClass* parent)
{
    using namespace vidlib;
    switch ( severity ) {
        case FFmpegVideoStream::LogSeverityQuiet:
            break;

        case FFmpegVideoStream::LogSeverityPanic:
        case FFmpegVideoStream::LogSeverityFatal:
        case FFmpegVideoStream::LogSeverityError:
            PLUGIN_LOG_NAMED_ERROR("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityWarning:
            PLUGIN_LOG_NAMED_WARNING("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityInfo:
        case FFmpegVideoStream::LogSeverityVerbose:
            PLUGIN_LOG_NAMED_INFO("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityDebug:
            PLUGIN_LOG_NAMED_DEBUG("ffmpeg", msg);
            break;

        default:
            UTILS_ASSERT(false, "Nieznany poziom FFmpeg. Wiadomość: %s", msg);
            break;
    }
}

static struct Initializer
{
    Initializer()
    {
        vidlib::FFmpegVideoStream::setLogCallback(/*vidlib::FFmpegImageStream::logCallback*/logCallback);
        vidlib::FFmpegVideoStream::setLockManager(vidlib::FFmpegImageStream::lockManager);
    }
} initializer;
