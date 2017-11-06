/********************************************************************
	created:  2010/07/29
	created:  29:7:2010   1:03
	filename: Converter.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__CONVERTER_H__
#define HEADER_GUARD_VIDLIB__CONVERTER_H__

#include <vidlib/PixelFormat.h>
#include <vidlib/Picture.h>

struct SwsContext;

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

/**
 *	
 */
class Converter
{
private:
  //! Opis konwersji.
  struct Description 
  {
    //! Szerokość źródłowego obrazka.
    int srcWidth;
    //! Wysokość źródłowgo obrazka.
    int srcHeight;
    //! Format źródłowego obrazka
    VIDLIB_PixelFormat srcFormat;
    //! Szerokość docelowego obrazka.
    int dstWidth;
    //! Wysokość docelowego obrazka.
    int dstHeight;
    //! Format docelowego obrazka
    VIDLIB_PixelFormat dstFormat;
    //! Operator mniejszości, wykorzystywany w kontenerze std::map
    bool operator<(const Description& rhs) const
    {
      return ::memcmp(this, &rhs, sizeof(*this)) < 0;
    }
  };

private:
  //! Cache kontekstów konwersji.
  std::map<Description, SwsContext*> cache;

public:
  //! 
  //! \param src
  //! \param dst
  bool convert( Picture src, Picture dst );
  //! 
  //! \param src
  //! \param dst
  bool convert( PictureLayered src, PictureLayered dst );
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__CONVERTER_H__
