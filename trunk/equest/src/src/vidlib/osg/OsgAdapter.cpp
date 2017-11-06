#include "../PCH.h"
#include "../VidLibPrivate.h"
#include <vidlib/osg/OsgAdapter.h>
#include <boost/foreach.hpp>
#include <utils/PtrPolicyOSG.h>

namespace vidlib 
{
    
    __OsgAdapter::__OsgAdapter() :
    stream(nullptr)
    {
    }

    __OsgAdapter::~__OsgAdapter()
    {
    }

    void __OsgAdapter::refreshImages()
    {
        BOOST_FOREACH( const Images::value_type& val, images ) {
            osg::ref_ptr<VideoImage> img;
            if (val.second.image.lock(img)) {
                img->refresh();
            }
        }
    }

    osg::ref_ptr<VideoImage> __OsgAdapter::getImage( PixelFormat format ) const
    {
        Entry& entry = getEntry(format);
        return getImage(entry, format);
    }

    osg::ref_ptr<osg::Texture2D> __OsgAdapter::getTexture2D( PixelFormat format ) const
    {
        Entry& entry = getEntry(format);
        osg::ref_ptr<osg::Texture2D> texture;
        if ( entry.texture2D.lock(texture) ) {
            return texture;
        } else {
            texture = new osg::Texture2D(getImage(entry, format));
            texture->setResizeNonPowerOfTwoHint(false);
            entry.texture2D = texture;
            return texture;
        }
    }

    osg::ref_ptr<osg::Texture> __OsgAdapter::getTexture( PixelFormat format, bool textureRect ) const
    {
        if ( textureRect ) {
            return getTextureRect(format);
        } else {
            return getTexture2D(format);
        }
    }

    osg::ref_ptr<osg::TextureRectangle> __OsgAdapter::getTextureRect( PixelFormat format ) const
    {
        Entry& entry = getEntry(format);
        osg::ref_ptr<osg::TextureRectangle> texture;
        if ( entry.textureRect.lock(texture) ) {
            return texture;
        } else {
            texture = new osg::TextureRectangle(getImage(entry, format));
            entry.textureRect = texture;
            return texture;
        }
    }

    __OsgAdapter::Entry& __OsgAdapter::getEntry( PixelFormat format ) const
    {
        return images[format];
    }

    osg::ref_ptr<VideoImage> __OsgAdapter::getImage( Entry &entry, PixelFormat format ) const
    {
        osg::ref_ptr<VideoImage> image;
        if ( entry.image.lock(image) ) {
            return image;
        } else {
            image = createImage(format);
            entry.image = image;
            return image;
        }
    }

    inline VideoStream* __OsgAdapter::asStream() const
    {
        if ( !stream ) {
            stream = dynamic_cast<VideoStream*>(const_cast<__OsgAdapter*>(this));
            UTILS_ASSERT(stream);
        }
        return stream;
    }

    osg::ref_ptr<VideoImage> __OsgAdapter::createImage( PixelFormat format ) const
    {
        return new VideoImage(asStream(), format);
    }

    OsgStream::OsgStream( VideoStream* innerStream ) :
    VideoStreamAdapter(innerStream), currentTimestamp(INVALID_TIMESTAMP)
    {
    }

    OsgStream::OsgStream( const OsgStream& stream ) : 
    VideoStreamAdapter(stream), currentTimestamp(stream.currentTimestamp)
    {
    }

    OsgStream::~OsgStream()
    {
    }

    VideoStream* OsgStream::clone() const
    {
        return new OsgStream(*this);
    }

    bool OsgStream::setTime( double time )
    {
        if ( VideoStreamAdapter::setTime(time) ) {
            double timestamp = getFrameTimestamp();
            if (timestamp != currentTimestamp) {
                currentTimestamp = timestamp;
                refreshImages();
            }
            return true;
        } else {
            VIDLIB_ERROR(VideoError("Error setting inner stream time."));
        }
    }

    osg::ref_ptr<VideoImage> OsgStream::createImage( PixelFormat format ) const
    {
        osg::ref_ptr<OsgStream> ptr(const_cast<OsgStream*>(this));
        return new VideoImageSafe<OsgStream, utils::PtrPolicyOSG>(ptr, format);
    }
} // namespace vidlib
