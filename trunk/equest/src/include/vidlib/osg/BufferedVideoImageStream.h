/********************************************************************
	created:  2010/07/22
	created:  22:7:2010   10:25
	filename: BufferedImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__BUFFEREDVIDEOIMAGESTREAM_H__
#define HEADER_GUARD_VIDLIB__BUFFEREDVIDEOIMAGESTREAM_H__

#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osg/observer_ptr>
UTILS_POP_WARNINGS
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/VideoBuffer.h>
#include <vidlib/Config.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////
UTILS_PUSH_WARNINGS
UTILS_DISABLE_DLL_INTERFACE_WARNING

/**
 *	Strumień obrazków z z buforowaniem.
 */
class VIDLIB_EXPORT BufferedVideoImageStream : public VideoImageStream
{
private:
  //! Wątek buforujący.
  class Bufferer
  {
  private:
    //! Strumień buforowany.
    osg::observer_ptr<BufferedVideoImageStream> stream;
  public:
    //! Konstruktor.
    Bufferer(BufferedVideoImageStream * stream);
    //! Funkcja buforująca.
    void run();
  };
  //! Wrapper bufora ramek.
  typedef utils::PtrWrapper<VideoBuffer, utils::PtrPolicyOSG> FrameBuffer;

private:
  //! Bufor ramek.
  osg::ref_ptr<FrameBuffer> frameBuffer;
  //! Czas strumienia.
  double wantedTime;

public:
  /** Konstruktor zerujący */
  BufferedVideoImageStream();
  /** Konstruktor kopiujący */
  BufferedVideoImageStream(const BufferedVideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
  /** Domyślne implementacje */
  META_Object(vidlib, BufferedVideoImageStream);

  /** Otwiera strumień */
  virtual bool open(const std::string & filename);

protected:
  /** Destruktor */
  virtual ~BufferedVideoImageStream();

public:
  /** Czas w strumieniu */
  virtual double getReferenceTime() const;


  /** Ustawia bieżącą ramkę (jeśli jest nowsza niż poprzednia)*/
  virtual void publishFrame();
  /** Ustawia bieżący czas */
  virtual void setStreamTime( double time );


};

UTILS_POP_WARNINGS
////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__BUFFEREDVIDEOIMAGESTREAM_H__
