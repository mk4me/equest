#include "PCH.h"
#include <vidlib/Picture.h>
#include "VidLibPrivate.h"

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//!
struct PictureContext
{
  //! Funkcja konwertująca obrazek do postaci planarnej.
  typedef PictureLayered (*ConvertToLayers)(Picture& src);
  //! Funkcja tworząca pusty obrazek.
  typedef void (*InitPictureDataSize)(Picture& src);
  //!
  typedef void (*InitLayersDataSize)(PictureLayered& src);

  //! Funkcja konwertująca obrazek do postaci planarnej.
  ConvertToLayers convertToLayers;
  //! Funkcja tworząca obrazek.
  InitPictureDataSize initPictureDataSize;
};

//------------------------------------------------------------------------------

template <int BPP>
void getDataSizeSimple(Picture & img)
{
  VIDLIB_FUNCTION_PROLOG;
  img.dataWidth = img.width * BPP;
  img.dataHeight = img.height;
}

template <int BPP>
void getLayersDataSizeSimple(PictureLayered & img)
{
  VIDLIB_FUNCTION_PROLOG;
  img.dataWidth[0] = img.width * BPP;
  img.dataHeight[0] = img.height;
}

void getDataSizeYV12(Picture & img)
{
  VIDLIB_FUNCTION_PROLOG;
  UTILS_ASSERT( !(img.height & 0x01) );
  UTILS_ASSERT( !(img.width & 0x01) );
  img.dataWidth = img.width;
  img.dataHeight = img.height + (img.height>>1);
}

//------------------------------------------------------------------------------

PictureLayered layeredFromSimple(Picture & src)
{
  VIDLIB_FUNCTION_PROLOG;
  PictureLayered result;
  utils::zero(result);
  // skopiowanie danych
  result.format = src.format;
  result.width = src.width;
  result.height = src.height;
  // ustawienie warstw
  result.data[0] = src.data;
  result.dataWidth[0] = src.dataWidth;
  result.dataHeight[0] = src.dataHeight;
  return result;
}

PictureLayered layeredFromYV12(Picture & src)
{
  VIDLIB_FUNCTION_PROLOG;
  UTILS_ASSERT( !(src.dataWidth & 0x01) );
  UTILS_ASSERT( !(src.height & 0x01) );

  PictureLayered result;
  utils::zero(result);
  // skopiowanie danych
  result.format = src.format;
  result.width = src.width;
  result.height = src.height;
  // ustawienie warstw
  result.data[0] = src.data;
  // warstwa U znajduje się poniżej warswy Y (src.height == 0.66 * src.dataHeight)
  result.data[1] = result.data[0] + ( src.dataWidth * src.height );
  // w połowie wiersza zaczyna się warstwa V
  result.data[2] = result.data[1] + ( src.dataWidth >> 1);
  //
  result.dataWidth[0] = src.dataWidth;
  result.dataWidth[1] = src.dataWidth;
  result.dataWidth[2] = src.dataWidth;
  result.dataHeight[0] = src.height;
  result.dataHeight[1] = src.height>>1;
  result.dataHeight[2] = src.height>>1;
  return result;
}

//------------------------------------------------------------------------------

//!
static PictureContext ContextRGB24 = {
  layeredFromSimple, getDataSizeSimple<3>
};

//!
static PictureContext ContextBGRA32 = {
  layeredFromSimple, getDataSizeSimple<4>
};

//!
static PictureContext ContextYV12 = {
  layeredFromYV12, getDataSizeYV12
};

//------------------------------------------------------------------------------

const PictureContext * PictureCommon::getContext() const
{
  VIDLIB_FUNCTION_PROLOG;
  // wybieramy kontekst
  switch ( format ) {
    case PixelFormatRGB24:
      return &ContextRGB24;
    case PixelFormatBGRA:
      return &ContextBGRA32;
    case PixelFormatYV12:
      return &ContextYV12;
    default:
      UTILS_FAIL("Format not supported.");
      return NULL;
  }
}

//------------------------------------------------------------------------------

Picture Picture::create( int width, int height, VIDLIB_PixelFormat format )
{
  VIDLIB_FUNCTION_PROLOG;
  // tworzenie obrazka
  Picture result;
  utils::zero(result);
  result.format = format;
  result.width = width;
  result.height = height;
  result.alloc();
  return result;
}


int Picture::getAllocSize( int width, int height, VIDLIB_PixelFormat format )
{
  VIDLIB_FUNCTION_PROLOG;
  Picture temp;
  temp.width = width;
  temp.height = height;
  temp.format = format;
  temp.getContext()->initPictureDataSize(temp);
  return temp.getAllocSize();
}

PictureLayered Picture::getLayers()
{
  VIDLIB_FUNCTION_PROLOG;
  UTILS_ASSERT(getContext());
  //Picture pic = create(dataWidth, dataHeight, format);
  return getContext()->convertToLayers(*this);
}

void Picture::copyTo( Picture & dst ) const
{
  VIDLIB_FUNCTION_PROLOG;
  UTILS_ASSERT( dataWidth == dst.dataWidth );
  UTILS_ASSERT( dataHeight == dst.dataHeight );
  ::memcpy( dst.data, data, dataWidth * dataHeight );
}


void Picture::alloc()
{
  VIDLIB_FUNCTION_PROLOG;
  getContext()->initPictureDataSize(*this);
  data = new unsigned char[dataHeight * dataWidth];
}

void Picture::free()
{
  VIDLIB_FUNCTION_PROLOG;
  utils::deletePtr(data);
  utils::zero(*this);
}

void Picture::zero()
{
  VIDLIB_FUNCTION_PROLOG;
  ::memset(data, 0, dataWidth * dataHeight);
}

vidlib::PictureRGB PictureRGB::create( int width, int height )
{
    PictureRGB result;
    Picture& resultRef = result;
    resultRef = Picture::create(width, height, PixelFormatRGB24);
    return result;
}

int PictureRGB::getAllocSize( int width, int height )
{
    return Picture::getAllocSize(width, height, PixelFormatRGB24);
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
