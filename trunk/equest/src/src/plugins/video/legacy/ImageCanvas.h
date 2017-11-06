/********************************************************************
	created:  2010/07/13
	created:  13:7:2010   22:04
	filename: ImageCanvas.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__IMAGECANVAS_H__
#define __HEADER_GUARD__IMAGECANVAS_H__

#include <osg/Image>
#include <osgWidget/Widget>
#include <osgWidget/Canvas>
#include <osgWidget/Label>
#include <core/Enumerators.h>
#include <core/OsgWidgetBorder.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class ImageCanvas : public osgWidget::Canvas
{
private:
  //! Obrazek.
  osg::ref_ptr<osg::Image> image;
  //! Prostokąt zawierający obrazek.
  osg::ref_ptr<osgWidget::Widget> rect;
  //! Czy rozmiar ma odpowiadać rozmiarowi obrazka?
  bool keepImageRatio;
  //!
  core::OsgWidgetBorder border;

public:
  META_Object(osgUI, ImageCanvas)

  //! Konstruktor domyślny.
  //! \param Nazwa kontrolki.
  ImageCanvas(const std::string& name = "");

  //! Konstruktor kopiujący.
  //! \param canvas
  //! \param copyop
  ImageCanvas(const ImageCanvas& canvas, const osg::CopyOp& copyop);

  virtual ~ImageCanvas();

  //! \param maxWidth
  //! \param maxHeight
  //! \return Rozmiar (przeważnie mniejszy) zgodny z proporcjami obrazka.
  osgWidget::XYCoord getRationedSize(float maxWidth, float maxHeight);


  //! \return
  core::OsgWidgetBorder* getBorder()
  { 
      return &border;
  }
  //! \return
  osgWidget::Label * getLabel()
  {
    return border.getLabel();
  }
  //! \return
  osgWidget::Widget * getRect()
  {
    return rect.get();
  }
  //! \return
  const osgWidget::Label * getLabel() const
  {
    return border.getLabel();
  }
  //! \return
  const osgWidget::Widget * getRect() const
  {
    return rect.get();
  }
  //! \return
  inline const osg::Image* getImage() const
  { 
    return image.get();
  }
  //! \return
  inline osg::Image* getImage()
  { 
    return image.get();
  }
  //! \param image
  void setImage(osg::Image* image, bool useTextureRect = false);

  //! Ustawia współrzędne tekstury.
  //! \param normCoords Znormalizowane współrzędne.
  void setTexCoord(osgWidget::Quad normCoords = osgWidget::Quad(0,0,1,1) );
  
  //! \return
  inline bool getKeepImageRatio() const
  { 
    return keepImageRatio;
  }
  //! \param keepImageRatio
  void setKeepImageRatio(bool keepImageRatio);

protected:
  //! Przeładowanie metody rozmieszczającej elementy.
  //! \param diffWidth
  //! \param diffHeight
  virtual void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight );
  //! Przeładowanie metody obliczającej rozmiar.
  virtual osgWidget::Window::Sizes _getWidthImplementation() const;
  //! Przeładowanie metody obliczającej rozmiar.
  virtual osgWidget::Window::Sizes _getHeightImplementation() const;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__IMAGECANVAS_H__
