#ifndef HEADER_GUARD_VIDLIB__PIXELFORMAT_H__
#define HEADER_GUARD_VIDLIB__PIXELFORMAT_H__

#include <utils/Enum.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! Definicja formatów danych
#define VIDLIB_PIXELFORMAT_DEFINITION \
  PixelFormatYV12,\
  PixelFormatRESERVED0,\
  PixelFormatRGB24,\
  PixelFormatRESERVED1,\
  PixelFormatARGB = 27,\
  PixelFormatBGRA = 30,\
  PixelFormatUndefined

//! Format danych.
enum VIDLIB_PixelFormat {
  VIDLIB_PIXELFORMAT_DEFINITION
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

DECLARE_ENUM_ENCHANCEMENT(vidlib::VIDLIB_PixelFormat, VIDLIB_PIXELFORMAT_DEFINITION)

#endif  // HEADER_GUARD_VIDLIB__PIXELFORMAT_H__
