/********************************************************************
    created:  2011/03/29
    created:  29:3:2011   14:22
    filename: VideoImage.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOIMAGE_H__
#define HEADER_GUARD_VIDLIB__VIDEOIMAGE_H__

#include <vidlib/Config.h>
#include <utils/Utils.h>
#include <map>
UTILS_PUSH_WARNINGS
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Image>
UTILS_POP_WARNINGS
#include <vidlib/PixelFormat.h>
#include <vidlib/VideoStream.h>
#include <vidlib/Picture.h>

namespace vidlib 
{
    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING

    class VIDLIB_EXPORT VideoImage : public osg::Image
    {
    private:
        //! Klienci maksymalnego rozmiaru.
        typedef std::map<osg::observer_ptr<osg::Referenced>, int> SizeClients;
        //! Klienci maksymalnego rozmiaru.
        SizeClients sizes;
        //! Minimalna szerokość.
        int minWidth;
        //! Strumień video.
        VideoStream* stream;
        //! Obrazek.
        Picture currentPicture;
        //!
        double timestamp;

    public:
        //! \param stream
        //! \param format
        VideoImage(VideoStream* stream, VIDLIB_PixelFormat format);
        //!
        virtual ~VideoImage();

    public:
        //! \return Format strumienia.
        inline const VIDLIB_PixelFormat getFormat() const
        { 
            return currentPicture.format;
        }
        //! \return Wewnętrzny strumień video.
        inline VideoStream* getStream()
        { 
            return stream;
        }
        //! \return Wewnętrzny strumień video.
        inline const VideoStream* getStream() const
        { 
            return stream;
        }
        //! \return Minimalna szerokość.
        inline int getMinWidth() const
        { 
            return minWidth;
        }
        //! \return
        inline double getTimestamp() const
        { 
            return timestamp;
        }


        //! \param minWidth Minimalna szerokość. (>= 2)
        void setMinWidth(int minWidth);
        //! Usuwa klienta maksymalnej szerokości.
        bool removeMaxWidth(osg::Referenced* source);
        //! Dodaje klienta maksymalnej szerokości.
        //! \param source Źródło.
        //! \param maxWidth Szerokość.
        void setMaxWidth(osg::Referenced* source, int maxWidth);

        
        //! \return Bieżąca maksymalna szerokość.
        int getMaxWidth();
        //! Odświeża obrazek.
        void refresh();

    private:
        //! Przeładowuje obrazek.
        void reloadImage(Picture & picture);
        //!
        bool applyMaxWidth( int maxWidth );
    };
    
    //! Uzupełnienie poprzedniego 
    template <class T, class PtrPolicy>
    class VideoImageSafe : public VideoImage, public PtrPolicy
    {
    public:
        typedef typename PtrPolicy::template Ptr<T>::Type Ptr;

    private:
        Ptr referenceHolder;
        
    public:
        VideoImageSafe(const Ptr& ptr, VIDLIB_PixelFormat format) : 
        VideoImage( &*ptr, format ), referenceHolder(ptr)
        {}

        virtual ~VideoImageSafe() 
        {}

    };

    UTILS_POP_WARNINGS
} // namespace vidlib

#endif  // HEADER_GUARD_VIDLIB__VIDEOIMAGE_H__
