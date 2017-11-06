/********************************************************************
    created:  2011/04/12
    created:  12:4:2011   21:20
    filename: VideoStreamAdapter.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOSTREAMADAPTER_H__
#define HEADER_GUARD_VIDLIB__VIDEOSTREAMADAPTER_H__

#include <vidlib/VideoStream.h>

namespace vidlib 
{
    //! Adapter strumienia, umożliwiający zunifikowany dostęp do strumieni
    //! kontrolujących inne strumienie (bufory, osg)
    class VIDLIB_EXPORT VideoStreamAdapter : public VideoStream
    {
    private:
        //! Wewnętrzny strumień.
        VideoStream* innerStream;

    public:
        //! \param innerStream Strumień przejmowany na własność.
        VideoStreamAdapter(VideoStream* innerStream);
        //!
        virtual ~VideoStreamAdapter();

    protected:
        //! Prywatny konstruktor do klonowania. Wykonuje głęboką kopię.
        VideoStreamAdapter(const VideoStreamAdapter& adapter);

    public:
        //! \return Wewnętrzny strumień.
        inline const VideoStream* getInnerStream() const
        {
            return innerStream;
        }

    // VideoStream
    public:
        //!
        virtual VideoStream* clone() const;
        //! \param time Pozycja w źródle
        virtual bool setTime(double time);
        //! Odczytuje następną klatkę.
        virtual bool readNext();
        //! \retrun Pozycja w strumieniu.
        virtual double getTime() const;
        //! \return Prawdziwy timestamp ramki.
        virtual double getFrameTimestamp() const;
        //! \return Prawdziwy timestamp kolejnej ramki.
        virtual double getNextFrameTimestamp() const;
        //!
        virtual bool getData(Picture& dst) const;
        //!
        virtual bool getData(PictureLayered& dst) const;
    };
} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__VIDEOSTREAMADAPTER_H__
