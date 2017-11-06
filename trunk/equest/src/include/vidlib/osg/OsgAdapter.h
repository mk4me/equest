/********************************************************************
    created:  2011/03/29
    created:  29:3:2011   14:12
    filename: OsgAdapter.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__
#define HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__

#include <map>
#include <string>
#include <utils/Utils.h>

UTILS_PUSH_WARNINGS
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
UTILS_POP_WARNINGS

#include <vidlib/VideoStream.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/osg/VideoImage.h>
#include <vidlib/VideoStreamAdapter.h>

namespace vidlib 
{
    class OsgStream;

    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING
    //! Adapter strumienia z vidliba dla potrzeb OSG. Potrafi optymalizować dostęp do obrazków.
    class VIDLIB_EXPORT __OsgAdapter
    {
        friend class OsgStream;
    private:
        //! Informacje dotyczące użycia strumienia.
        struct Entry
        {
            osg::observer_ptr<VideoImage> image;
            osg::observer_ptr<osg::Texture2D> texture2D;
            osg::observer_ptr<osg::TextureRectangle> textureRect;
        };
        //! Obrazki powstałe na bazie strumienia.
        typedef std::map< VIDLIB_PixelFormat, Entry> Images;
        //! Obrazki powstałe na bazie strumienia.
        mutable Images images;
        //!
        mutable VideoStream* stream;

    protected:
        //! Konstruktor zerujący.
        __OsgAdapter();
        //! Pusty polimorficzny destruktor.
        virtual ~__OsgAdapter();

    public:
        //! \return Obrazek dla zadanego formatu.
        osg::ref_ptr<VideoImage> getImage(VIDLIB_PixelFormat format) const;

        //!
        osg::ref_ptr<osg::Texture> getTexture(VIDLIB_PixelFormat format, bool textureRect) const;
        //! \return Obrazek dla zadanego formatu.
        osg::ref_ptr<osg::Texture2D> getTexture2D(VIDLIB_PixelFormat format) const;
        //! \return Obrazek dla zadanego formatu.
        osg::ref_ptr<osg::TextureRectangle> getTextureRect(VIDLIB_PixelFormat format) const;


    protected:
        //! Odświeża obrazki korzystające z danego strumienia.
        void refreshImages();

        virtual osg::ref_ptr<VideoImage> createImage(VIDLIB_PixelFormat format) const;

    private:
        //!
        Entry& getEntry(VIDLIB_PixelFormat format) const;
        //!
        osg::ref_ptr<VideoImage> getImage( Entry &entry, VIDLIB_PixelFormat format ) const;
        //!
        VideoStream* asStream() const;
    };
    UTILS_POP_WARNINGS

    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING
    //! Metody z premedytacją nie są wirtualne tak, aby odwołania do nich (znając typ OsgStream)
    //! mogły być wydajniejsze. Oczywiście NIE powinno się dziedziczyć po tym typie.
    class VIDLIB_EXPORT OsgStream : public VideoStreamAdapter, public __OsgAdapter, public osg::Referenced
    {
    private:
        double currentTimestamp;

    public:
        //! \param innerStream Strumień przejmowany na własność.
        OsgStream(VideoStream* innerStream);
        //!
        virtual ~OsgStream();

    protected:
        //!
        OsgStream(const OsgStream& stream);

    // VideoStream
    public:
        //!
        virtual OsgStream* clone() const;
        //! \param time Pozycja w źródle
        bool setTime(double time);

    protected:
        virtual osg::ref_ptr<VideoImage> createImage(VIDLIB_PixelFormat format) const;
    };

    //! Adapter strumienia z vidliba dla potrzeb OSG.
    template <class Stream>
    class OsgAdapter : public Stream, public __OsgAdapter
    {
    public:

        //! Konstruktor.
        OsgAdapter(const std::string& source) :
        Stream(source)
        {}
        //! Pusty polimorficzny destruktor.
        virtual ~OsgAdapter()
        {}

        //! Aktualizuje obrazki korzystające ze strumienia.
        virtual bool setTime(double time)
        {
            double prevTimestamp = this->Stream().getFrameTimestamp();
            bool result = Stream::setTime(time);
            if ( prevTimestamp != this->Stream().getFrameTimestamp() ) {
                refreshImages();
            }
            return result;
        }

    };
    UTILS_POP_WARNINGS

} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__
