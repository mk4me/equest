#include "VideoPCH.h"
#include <osg/TextureRectangle>
#include "ImageCanvas.h"

#undef min
#undef max

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

ImageCanvas::ImageCanvas( const std::string& name/*= ""*/ ) :
  osgWidget::Canvas(name),
  keepImageRatio(false)
{
  // tło przeźroczyste
  getBackground()->setColor(0, 0, 0, 0);

  // tworzymy widget
  rect = new osgWidget::Widget("image", 64, 64);
  rect->setPadding(1);
  rect->setLayer(osgWidget::Widget::LAYER_MIDDLE);
  rect->setColor(1.0, 1.0, 1.0, 1.0);
  rect->setEventMask(osgWidget::EVENT_MOUSE_SCROLL);

  // dodajemy widgety
  addWidget(rect, 0, 0);

  border.setParent(this);
  border.setShowBorder(true);
  border.setShowLabel(true);
}

ImageCanvas::ImageCanvas( const ImageCanvas& canvas, const osg::CopyOp& copyop ):
  osgWidget::Canvas(canvas, copyop),
  keepImageRatio(canvas.keepImageRatio)
{
  rect = getByName("image");
  border.setParent(this);
  border.onClone(canvas.border);

  // ustawiamy zmienną image
  osg::StateSet* ss = rect->getStateSet();
  if (ss) {
    osg::Texture * texture = dynamic_cast<osg::Texture*>(ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    if ( texture ) {
      image = texture->getImage(0);
    }
  }
}

ImageCanvas::~ImageCanvas()
{

}

osgWidget::XYCoord ImageCanvas::getRationedSize( float maxWidth, float maxHeight )
{
  float rectRatio = maxWidth / maxHeight;
  float imageRatio = (image->s()* image->getPixelAspectRatio()) / image->t() ;
  if ( imageRatio > rectRatio ) {
    maxWidth = ceil(maxWidth);
    maxHeight = ceil(maxWidth / imageRatio);
  } else {
    maxHeight = ceil(maxHeight);
    maxWidth = ceil(maxHeight * imageRatio);
  }
  return osgWidget::XYCoord(maxWidth, maxHeight);
}

void ImageCanvas::setImage( osg::Image* image, bool useTextureRect /*= false*/ )
{
  this->image = image;
  if ( image ) {
    osg::Texture * texture = NULL;
    if ( useTextureRect ) {
      texture = new osg::TextureRectangle();
      texture->setImage(0, image);
    } else {
      // tworzymy teksturkę
      texture = new osg::Texture2D();
      texture->setImage(0, image);
      texture->setResizeNonPowerOfTwoHint(false);
    }
    // ustawiamy teksturkę
    rect->setTexture(texture, false, false);
    // ustawiamy współrzędne
    setTexCoord();
    getLabel()->setLabel(image->getFileName());
  } else {
    rect->setTexture(NULL);
    getLabel()->setLabel("NONE");
  }
}

void ImageCanvas::_resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
{
  osgWidget::Canvas::_resizeImplementation(diffWidth, diffHeight);
  // aktualizacja rozmiaru prostokąta z obrazkiem
  float height = getHeight() + diffHeight;
  float width = getWidth() + diffWidth;

  if ( getKeepImageRatio() ) {
    osgWidget::XYCoord size = getRationedSize(width, height);
    width = size.x();
    height = size.y();
  }

  // ustawienie rozmiaru obrazka
  rect->setSize(width - rect->getPadHorizontal(), height - rect->getPadVertical());
  // pozycja
  rect->setOrigin( rect->getPadLeft(), rect->getPadBottom() );
  // aktualizacja pozycji labelki


  // resize ramki
  border.setLocation( 0, 0, width, height, rect->getPadRight() );
}

osgWidget::Window::Sizes ImageCanvas::_getWidthImplementation() const
{
  osgWidget::Window::Sizes result;
  result.current = rect->getWidthTotal();
  result.minimum = rect->getPadHorizontal();
  result.minimum = std::max(0.0f, result.minimum);
  result.current = std::max(result.current, result.minimum);
  return result;
}

osgWidget::Window::Sizes ImageCanvas::_getHeightImplementation() const
{
  osgWidget::Window::Sizes result;
  result.current = rect->getHeightTotal();
  result.minimum = rect->getPadVertical();
  result.minimum = std::max(0.0f, result.minimum);
  result.current = std::max(result.current, result.minimum);
  return result;
}

void ImageCanvas::setKeepImageRatio( bool keepImageRatio )
{
  this->keepImageRatio = keepImageRatio;
  if ( keepImageRatio ) {
    resizeAdd();
  }
}

void ImageCanvas::setTexCoord(osgWidget::Quad normCoords /*= osgWidget::Quad(0,0,1,1)*/ )
{
  // ustawiamy zmienną image
  osg::StateSet* ss = rect->getStateSet();
  bool useTextureRect = ss && dynamic_cast<osg::TextureRectangle*>(ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
  bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
  double left, right, top, bottom;
  double width, height;
  if ( useTextureRect ) {
    width = image->s();
    height = image->t();
  } else {
    width = 1.0;
    height = 1.0;
  }

  bottom = (flip ? normCoords.w() : normCoords.y()) * height;
  top = (flip ? normCoords.y() : normCoords.w()) * height;
  left = normCoords.x() * width;
  right = normCoords.z() * width;

  rect->setTexCoord(left, top, osgWidget::Widget::UPPER_LEFT);
  rect->setTexCoord(left, bottom, osgWidget::Widget::LOWER_LEFT);
  rect->setTexCoord(right, top, osgWidget::Widget::UPPER_RIGHT);
  rect->setTexCoord(right, bottom, osgWidget::Widget::LOWER_RIGHT);
}


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////
