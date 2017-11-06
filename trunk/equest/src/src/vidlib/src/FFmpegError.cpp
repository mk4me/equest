#include "PCH.h"
#include <vidlib/FFmpegError.h>
#include <utils/array_traits.h>

#if defined(_WINDOWS)
// blok definicji dla ffmpeg'a
#define inline _inline
#define snprintf _snprintf
#endif

extern "C" {
#pragma warning (push)
#pragma warning (disable: 4244)
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#undef INTMAX_C
#undef UINTMAX_C
#include <libavutil/mem.h>
#include <libavutil/error.h>
#pragma warning (pop)
}

#ifdef _MSC_VER
// wyłączamy warning bo wiemy co robimy
#pragma warning(disable: 4996)
#endif

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

FFmpegError::FFmpegError( const char* msg, int id /*= UnknownID*/ ) : 
VideoError(nullptr), id(id)
{
    setMessage(msg, -1);
}

FFmpegError::FFmpegError( const std::string& msg, int id /*= UnknownID*/ ) : 
VideoError(nullptr), id(id)
{
    setMessage(msg.c_str(), msg.length());
}

FFmpegError::FFmpegError( const FFmpegError& error ) : 
VideoError(error), id(error.id)
{}

VideoError * FFmpegError::clone() const
{
    return new FFmpegError(*this);
}

void FFmpegError::setMessage( const char* msg, size_t length )
{
    enum ErrorCode {
        ErrorCodeINVALIDDATA = AVERROR_INVALIDDATA,
        //ErrorCoderIO = AVERROR_IO,
        //ErrorCodeNOENT = AVERROR_NOENT,
        //ErrorCodeNOFMT = AVERROR_NOFMT,
        //ErrorCodeNOMEM = AVERROR_NOMEM,
        //ErrorCodeNOTSUPP = AVERROR_NOTSUPP,
        //ErrorCodeNUMEXPECTED = AVERROR_NUMEXPECTED,
        ErrorCodeUNKNOWN = AVERROR_UNKNOWN,
        ErrorCodeEOF = AVERROR_EOF,
        ErrorCodePATCHWELCOME = AVERROR_PATCHWELCOME
    };


    ErrorCode errorCode = static_cast<ErrorCode>(id);
    if ( id != UnknownID ) {
        // tłumaczymy błąd
        char buffer[512];
        static const size_t maxLength = utils::size(buffer) - 1;
        buffer[maxLength] = '\0';
        // najpierw kopiujemy starą wiadomość
        if ( msg ) {
            strncpy(buffer, msg, maxLength);
            length = strlen(buffer);
            strncpy(buffer + length, "\n", length < maxLength ? maxLength - length : 0);
            length = strlen(buffer);
        } else {
            length = 0;
        }
        // Dodajemy komunikat o błędzie:
        strncpy(buffer + length, "Internal FFmpeg error: ", length < maxLength ? maxLength - length : 0);
        length = strlen(buffer);
        av_strerror(static_cast<int>(errorCode), buffer+length, length < maxLength ? maxLength - length : 0);
        VideoError::setMessage(buffer, -1);
    } else {
        VideoError::setMessage(msg, length);
    }

}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
