#include "PCH.h"
#include <vidlib/VideoError.h>
#include <utils/Debug.h>

#ifdef _MSC_VER
// wyłączamy warning bo wiemy co robimy
#pragma warning(disable: 4996)
#endif

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

VideoError::VideoError(const char* msg) 
: msgCopy(nullptr)
{
    setMessage(msg, -1);
}

VideoError::VideoError(const std::string& msg) 
: msgCopy(nullptr)
{
    setMessage(msg.c_str(), msg.length());
}

VideoError::VideoError(const VideoError& error) 
: std::exception(error), msgCopy(nullptr)
{
    setMessage(error.what(), -1);
}

VideoError::~VideoError() throw()
{
    if ( msgCopy ) {
        free(msgCopy);
        msgCopy = nullptr;
    }
}

const char* VideoError::what() const throw()
{
    return msgCopy ? msgCopy : "Unknown exception";
}

VideoError * VideoError::clone() const
{
    return new VideoError(*this);
}

void VideoError::setMessage( const char* msg, size_t length ) 
{
    UTILS_ASSERT(!msgCopy);
    if (msg) {
        const size_t buffSize = (length == -1 ? strlen(msg) : length) + 1;
        msgCopy = static_cast<char*>(malloc(buffSize));
        if ( msgCopy ) {
            strcpy(msgCopy, msg);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
