/********************************************************************
	created:  2010/07/29
	created:  29:7:2010   1:03
	filename: Converter.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__AVIO_CONVERTER_H__
#define __HEADER_GUARD__AVIO_CONVERTER_H__

struct SwsContext;

////////////////////////////////////////////////////////////////////////////////
namespace video {
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
    PixelFormat srcFormat;
    //! Szerokość docelowego obrazka.
    int dstWidth;
    //! Wysokość docelowego obrazka.
    int dstHeight;
    //! Format docelowego obrazka
    PixelFormat dstFormat;
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
} // namespace video
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__AVIO_CONVERTER_H__
