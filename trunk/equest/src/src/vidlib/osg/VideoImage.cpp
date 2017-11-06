#include "../PCH.h"
#include <vidlib/osg/VideoImage.h>
#include <utils/Align.h>

namespace vidlib 
{
    VideoImage::VideoImage( VideoStream* stream, PixelFormat format ) :
    stream(stream), minWidth(2), timestamp(-1)
    {
        UTILS_ASSERT(stream);

        setOrigin(osg::Image::TOP_LEFT);

        if ( format == PixelFormatYV12 ) {
            // poniewaz wymagamy konwersji w shaderach, ustawiamy aspect ratio
            setPixelAspectRatio(static_cast<float>(stream->getAspectRatio()) * 1.5f);
        } else {
            setPixelAspectRatio(static_cast<float>(stream->getAspectRatio()));
        }

        utils::zero(currentPicture);
        currentPicture = Picture::create(stream->getWidth(), stream->getHeight(), format);
        reloadImage(currentPicture);
    }

    VideoImage::~VideoImage()
    {
        stream = nullptr;
        currentPicture.free();
    }

    void VideoImage::setMinWidth( int minWidth )
    {
        UTILS_ASSERT(minWidth >= 2);
        this->minWidth = minWidth;
    }

    bool VideoImage::removeMaxWidth( osg::Referenced* source )
    {
        auto it = sizes.find(source);
        if ( it != sizes.end() ) {
            sizes.erase(it);
            applyMaxWidth(getMaxWidth());
            return true;
        } else {
            return false;
        }
    }

    void VideoImage::setMaxWidth( osg::Referenced* source, int maxWidth )
    {
        UTILS_ASSERT(maxWidth > 0);
        sizes[source] = maxWidth;
        applyMaxWidth(getMaxWidth());
    }

    int VideoImage::getMaxWidth()
    {
        int maxWidth = 0;
        // usunięcie zbędnych wpisów
        for ( SizeClients::iterator it = sizes.begin(); it != sizes.end(); ) {
            const osg::observer_ptr<osg::Referenced>& obs = it->first;
            if ( !obs.valid() ) {
                SizeClients::iterator toErase = it++;
                sizes.erase(toErase);
            } else {
                maxWidth = std::max(maxWidth, it->second);
                ++it;
            }
        }
        if ( maxWidth > 0 ) {
            return maxWidth;
        } else {
            return stream->getWidth();
        }
    }

    void VideoImage::refresh()
    {
        if (!applyMaxWidth(getMaxWidth())) {
            reloadImage(currentPicture);
        }
    }

    void VideoImage::reloadImage( Picture & picture )
    {
        stream->getFrame(picture);

        switch (picture.format) {

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

        timestamp = stream->getFrameTimestamp();
    }

    bool VideoImage::applyMaxWidth( int maxWidth )
    {
        int width = 0;
        int height = 0;
        const int streamWidth = getStream()->getWidth();
        const int streamHeight = getStream()->getHeight();

        // jaka szerokość zostanie zastosowana?
        if ( maxWidth >= streamWidth ) {
            width = streamWidth;
            height = streamHeight;
        } else {
            const double divBias = 1.0/3.0;
            int widthDiv = static_cast<int>((static_cast<double>(streamWidth)/maxWidth + divBias));
            width = std::max(minWidth, streamWidth / widthDiv);
            height = std::max(minWidth, streamHeight / widthDiv);
            width = UTILS_ALIGN(width, 2);
            height = UTILS_ALIGN(height, 2);
        }

        // resize obrazka
        if ( currentPicture.width != width ) {
            OSG_NOTICE<<"VideoImage: Resizing from "<<currentPicture.width<<" to "<<width<<std::endl;
            PixelFormat format = currentPicture.format;
            currentPicture.free();
            currentPicture = Picture::create(width, height, format);
            reloadImage(currentPicture);
            return true;
        } else {
            return false;
        }
    }
} // namespace vidlib
