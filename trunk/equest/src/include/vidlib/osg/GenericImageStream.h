/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   15:15
	filename: GenericImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__GENERICIMAGESTREAM_H__
#define HEADER_GUARD_VIDLIB__GENERICIMAGESTREAM_H__

#include <vidlib/osg/VideoImageStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! ImageStream korzystający z FFmpegVideoStream
template <class InnerStream>
class VIDLIB_EXPORT GenericImageStream : public VideoImageStream
{
public:
    /** Domyślne implementacje */
    META_Object(vidlib, GenericImageStream);

    /** Konstruktor zerujący */
    GenericImageStream()
    {}

    /** Konstruktor kopiujący */
    GenericImageStream(const GenericImageStream& image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY) :
    VideoImageStream(image, copyop)
    {}

    /** Otwiera strumień */
    virtual bool open(const std::string & filename)
    {
        try {
            setStream( new InnerStream(filename) );
            return true;
        } catch ( VideoError& error ) {
            OSG_FATAL<<"open("<<filename<<") reported an error: "<< error.what() << std::endl;
            return false;
        }
    }
};


////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD_VIDLIB__GENERICIMAGESTREAM_H__
