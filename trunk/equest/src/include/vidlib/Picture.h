/********************************************************************
	created:  2010/07/29
	created:  29:7:2010   13:21
	filename: Picture.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__PICTURE_H__
#define HEADER_GUARD_VIDLIB__PICTURE_H__

#include <vidlib/Config.h>
#include <vidlib/PixelFormat.h>
#include <utils/Utils.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//!
struct PictureContext;

//! Wspólne dane wszystkich obrazków.
struct VIDLIB_EXPORT PictureCommon
{
  //! Kontekst zdjęcia.
  VIDLIB_PixelFormat format;
  //! Szerokość (tekstele).
  int width;
  //! Wysokość (tekstele).
  int height;

  //! \return Kontekst zdjęcia.
  const PictureContext * getContext() const;
};

//! Obrazek z wieloma warstwami.
struct VIDLIB_EXPORT PictureLayered : public PictureCommon
{
  //! Dane warstw.
  unsigned char * data[4];
  //! Szerokości warstw (w bajtach).
  int dataWidth[4];
  //! Wysokość warstw (liczba wierszy)
  int dataHeight[4];

  //! \return Łączny rozmiar zaalokowanej pamięci.
  inline int getAllocSize() const
  {
    int size = 0;
    for (int i = 0; i < 4; ++i) {
      size += dataWidth[i] * dataHeight[i];
    }
    return size;
  }
};

//! Obrazek z jedną warstwą (lub ze spakowanymi danymi).
struct VIDLIB_EXPORT Picture : public PictureCommon
{
  //! Szerokość wiersza (w bajtach). W przypadku spakowanych danych
  //! może odbiegać od szerokości obrazka.
  int dataWidth;
  //! Liczba wierszy danych. W przypadku spakowanych danych może odbiegać
  //! od wysokości obrazka.
  int dataHeight;
  //! Dane.
  unsigned char * data;

  //! Tworzy instancję obrazka.
  //! \param width Szerokość.
  //! \param height Wysokość.
  //! \param format Format.
  static Picture create(int width, int height, VIDLIB_PixelFormat format);
  //! 
  //! \param width
  //! \param height
  //! \param format
  static int getAllocSize(int width, int height, VIDLIB_PixelFormat format);

  //! Określa rozmiar potrzebnych danych oraz alokuje.
  void alloc();
  //! Zwalnia dane obrazka.
  void free();
  //! Zeruje dane obrazka.
  void zero();

  //! \return Rozmiar zaalokowanej pamięci.
  inline int getAllocSize() const
  {
    return dataHeight * dataWidth;
  }

  //! \return Reprezentacja obrazka z wydzielonymi warstwami.
  PictureLayered getLayers();
  //! Kopiuje dane obrazka do obrazka docelowego.
  //! \param dst Obrazek docelowy.
  void copyTo(Picture & dst) const;
};

struct VIDLIB_EXPORT PictureRGB : public Picture
{
    //! Tworzy instancję obrazka.
    //! \param width Szerokość.
    //! \param height Wysokość.
    //! \param format Format.
    static PictureRGB create(int width, int height);
    //! 
    //! \param width
    //! \param height
    //! \param format
    static int getAllocSize(int width, int height);
};

//! Obrazek z destruktorem; nadaje się tam, gdzie kod ma wiele punktów wyjścia
//! i ręczne zwalnianie może być problemem.
struct VIDLIB_EXPORT ScopedPicture : public Picture
{
    //! 
    //! \param width
    //! \param height
    //! \param format
    ScopedPicture(int width, int height, VIDLIB_PixelFormat format)
    {
        reinterpret_cast<Picture&>(*this) = create(width, height, format);
    }
    //! 
    ~ScopedPicture()
    {
        free();
    }

private:
    //! Uniemożliwone kopiowanie.
    ScopedPicture(const ScopedPicture& picture);
    //! Uniemożliwione kopiowanie.
    ScopedPicture& operator=(const ScopedPicture& picture);
};

//! Obrazek z destruktorem; nadaje się tam, gdzie kod ma wiele punktów wyjścia
//! i ręczne zwalnianie może być problemem.
struct VIDLIB_EXPORT ScopedPictureRGB : public PictureRGB
{
public:
    ScopedPictureRGB()
    {
        utils::zero(*this);
        format = PixelFormatRGB24;
    }
    //! 
    ~ScopedPictureRGB()
    {
        free();
    }

private:
    //! Uniemożliwone kopiowanie.
    ScopedPictureRGB(const ScopedPictureRGB& picture);
    //! Uniemożliwione kopiowanie.
    ScopedPictureRGB& operator=(const ScopedPictureRGB& picture);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD_VIDLIB__PICTURE_H__
