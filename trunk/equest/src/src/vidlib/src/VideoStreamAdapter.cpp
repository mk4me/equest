#include "PCH.h"
#include "VidLibPrivate.h"
#include <vidlib/VideoStreamAdapter.h>

namespace vidlib 
{
    VideoStreamAdapter::VideoStreamAdapter( VideoStream* innerStream ) :
    innerStream(innerStream)
    {
        if ( !innerStream ) {
            VIDLIB_ERROR_NORETURN(VideoError("innerStream==null"));
        }
        onAfterInit(innerStream->getSource(), innerStream->getFramerate(), 
            innerStream->getDuration(), innerStream->getPixelFormat(), 
            innerStream->getWidth(), innerStream->getHeight(), 
            innerStream->getAspectRatio());
    }

    VideoStreamAdapter::VideoStreamAdapter( const VideoStreamAdapter& adapter ) :
    innerStream( adapter.innerStream->clone() )
    {
        if ( !innerStream ) {
            VIDLIB_ERROR_NORETURN(VideoError("innerStream==null"));
        }
        onAfterInit(innerStream->getSource(), innerStream->getFramerate(), 
            innerStream->getDuration(), innerStream->getPixelFormat(), 
            innerStream->getWidth(), innerStream->getHeight(), 
            innerStream->getAspectRatio());
    }

    VideoStreamAdapter::~VideoStreamAdapter()
    {
        utils::deletePtr(innerStream);
    }

    VideoStream* VideoStreamAdapter::clone() const
    {
        return new VideoStreamAdapter(*this);
    }

    bool VideoStreamAdapter::setTime( double time )
    {
        return innerStream->setTime(time);
    }

    bool VideoStreamAdapter::readNext()
    {
        return innerStream->readNext();
    }

    double VideoStreamAdapter::getTime() const
    {
        return innerStream->getTime();
    }

    double VideoStreamAdapter::getFrameTimestamp() const
    {
        return innerStream->getFrameTimestamp();
    }

    double VideoStreamAdapter::getNextFrameTimestamp() const
    {
        return innerStream->getNextFrameTimestamp();
    }

    bool VideoStreamAdapter::getData( Picture& dst ) const
    {
        return innerStream->getData(dst);
    }

    bool VideoStreamAdapter::getData( PictureLayered& dst ) const
    {
        return innerStream->getData(dst);
    }
} // namespace vidlib
