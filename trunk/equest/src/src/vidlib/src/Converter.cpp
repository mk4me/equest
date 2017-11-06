#include "PCH.h"
#include "VidLibPrivate.h"
#include "Converter.h"

#if defined(_WINDOWS)
// blok definicji dla ffmpeg'a
#define inline _inline
#define snprintf _snprintf
#endif

extern "C" {
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#undef INTMAX_C
#undef UINTMAX_C
#pragma warning (push)
#pragma warning (disable: 4244)
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#pragma warning (pop)
}

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

bool Converter::convert( PictureLayered src, PictureLayered dst )
{
  VIDLIB_FUNCTION_PROLOG;
  if ( src.width == dst.width && src.height == dst.height && src.format == dst.format ) {

    VIDLIB_MEASURE_SCOPE(CopyFrame);
    AVPicture picture1, picture2;
    ::memcpy(picture1.data, src.data, sizeof(picture1.data));
    ::memcpy(picture1.linesize, src.dataWidth, sizeof(picture1.linesize));
    ::memcpy(picture2.data, dst.data, sizeof(picture2.data));
    ::memcpy(picture2.linesize, dst.dataWidth, sizeof(picture2.linesize));
    av_picture_copy(&picture2, &picture1, static_cast< ::PixelFormat>(src.format),
      src.width, src.height);
    return true;
  }

  // wyszukujemy odpowiedni kontekst
  Description description = {
    src.width, src.height, src.format,
    dst.width, dst.height, dst.format
  };

  // czy jest już odpowiedni konwerter?
  std::map<Description, SwsContext*>::iterator found = cache.find(description);
  if ( found == cache.end() ) {
    // wstawiamy!
	  found = cache.insert({ description, reinterpret_cast<SwsContext*>(NULL) }).first;
  }

  VIDLIB_MEASURE_SCOPE(ConvertFrame);
  // odświeżamy kontekst
  SwsContext *ctx = sws_getCachedContext( found->second,
    description.srcWidth, description.srcHeight, static_cast< ::PixelFormat>(description.srcFormat),
    description.dstWidth, description.dstHeight, static_cast< ::PixelFormat>(description.dstFormat),
    SWS_FAST_BILINEAR, NULL, NULL, NULL);

  // aktualizacja cache
  found->second = ctx;

  // konwersja właściwa
  if ( !ctx ) {
    // nie da się!
    return false;
  } else {
    int newHeight = sws_scale(ctx, src.data, src.dataWidth, 0, description.srcHeight, dst.data, dst.dataWidth );
    return description.dstHeight == newHeight;
  }
}

bool Converter::convert( Picture src, Picture dst )
{
  VIDLIB_FUNCTION_PROLOG;
  return convert( src.getLayers(), dst.getLayers() );
}
////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

