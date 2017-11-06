#include "VideoPCH.h"
#include <osgWidget/Box>
#include <iostream>
#include <algorithm>
#include "HorizontalScrollBar.h"

#undef min
#undef max

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

HorizontalScrollBar::HorizontalScrollBar( const std::string& name /*= ""*/ )
: osgWidget::Canvas(name), currentValue(0.0), trackExtension(0.0), draggedValue(0.0)
{
  // kursor
  cursor = new Button("cursor");
  cursor->addCallback( new osgWidget::Callback(&HorizontalScrollBar::onScrollDrag, this, osgWidget::EVENT_MOUSE_DRAG) );
  cursor->addCallback( new osgWidget::Callback(&HorizontalScrollBar::onScrollPushed, this, osgWidget::EVENT_MOUSE_PUSH) );
  cursor->setLayer(osgWidget::Widget::LAYER_HIGH, 10);
  cursor->setOrigin(0.0, 0.0);

  // track
  track = createBackgroundWidget("track", true);
  track->setEventMask(osgWidget::EVENT_MOUSE_PUSH);
  track->addCallback( new osgWidget::Callback(&HorizontalScrollBar::onTrackClicked, this, osgWidget::EVENT_MOUSE_PUSH));

  // lewa krawędź tracka
  borderLeft = createBackgroundWidget("borderLeft");
  // prawa krawędź tracka
  borderRight = createBackgroundWidget("borderRight");

  // podokienko z tłem
  osgWidget::Box * trackWindow = new osgWidget::Box("trackWindow");
  trackWindow->addWidget(borderLeft);
  trackWindow->addWidget(track);
  trackWindow->addWidget(borderRight);
  trackWindow->getBackground()->setColor(0.0, 0.0, 0.0, 0.0);
  body = trackWindow->embed();
  body->setColor(0.0, 0.0, 0.0, 0.0);

  addWidget(cursor, 0, 0);
  addWidget(body, 0, 0);

  // dodajemy
  setUpdateCallback(new NestedCallback<HorizontalScrollBar>(this));
}

void HorizontalScrollBar::setValue( double value )
{
  value = std::max(0.0, std::min(value, 1.0) );
  if ( currentValue != value ) {    
    currentValue = value;
    fireEvent(EventIDValueChanged, this, value);
  }
}

void HorizontalScrollBar::setCursorValue( double value )
{
  osgWidget::Quad bounds = getCursorBounds();
  double x = (1.0-value) * bounds.x() + value * ( bounds.x() + bounds.z() );
  double y = bounds.y() + bounds.w() * 0.5;
  cursor->setOrigin( x, y );
}

double HorizontalScrollBar::getCursorValue() const
{
  osgWidget::Quad bounds = getCursorBounds();
  double x = cursor->getX();
  return ( x - bounds.x() ) / ( bounds.z() );
}

osgWidget::Widget * HorizontalScrollBar::createBackgroundWidget( const std::string& name, bool canFill /*= false*/ )
{
  osgWidget::Widget * result = new osgWidget::Widget(name);
  result->setLayer(osgWidget::Widget::LAYER_LOW);
  result->setCanFill(canFill);
  return result;
}

osgWidget::Quad HorizontalScrollBar::getCursorBounds() const
{
  osgWidget::Quad result;
  // wysokość taka jak chcemy
  result.y() = track->getY();
  result.w() = track->getHeight() - cursor->getHeight();
  // szerokość domyślnie też
  result.x() = track->getX();
  result.z() = track->getWidth() - cursor->getWidth();
  //
  result.x() -= getTrackExtension();
  result.z() += 2 * getTrackExtension();
  return result;
}

bool HorizontalScrollBar::onScrollDrag( osgWidget::Event& event )
{
  double delta = event.x / getCursorBounds().z();
  draggedValue += delta;
  setValue( draggedValue );
  return false;
}

bool HorizontalScrollBar::onTrackClicked( osgWidget::Event& event )
{
  osgWidget::Quad bounds = getCursorBounds();
  double shifted = localXY(event.x,event.y).x() - cursor->getWidth() / 2.0;
  setValue( (shifted - bounds.x()) / bounds.z() );
  return false;
}

void HorizontalScrollBar::_resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
{
  //std::cout << diffWidth << "; " << diffHeight << std::endl;
  osgWidget::Canvas::_resizeImplementation(diffWidth, diffHeight);
  // aktualizacja pozycji kursora
  setCursorValue(currentValue);
  // aktualizacja ciala
  double height = getHeight() + diffHeight;
  double width = getWidth() + diffWidth;
  borderLeft->setHeight(height);
  borderRight->setHeight(height);
  body->setSize(width, height);
}

osgWidget::Window::Sizes HorizontalScrollBar::_getWidthImplementation() const
{
  osgWidget::Window::Sizes result;
  result.current = body->getWidth();
  result.minimum = cursor->getWidth() + borderLeft->getWidth() + borderRight->getWidth();
  result.minimum = ceil(result.minimum);
  result.current = std::max(result.current, result.minimum);
  return result;
}

osgWidget::Window::Sizes HorizontalScrollBar::_getHeightImplementation() const
{
  osgWidget::Window::Sizes result;
  result.current = body->getHeight();
  result.minimum = 16;
  result.current = std::max(result.current, result.minimum);
  return result;
}

void HorizontalScrollBar::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
  if ( currentValue != getCursorValue() ) {
    setCursorValue(currentValue);
  }

  if ( tooltipWindow ) {
    if ( cursor->isHovered() ) {
      if ( !tooltipWindow->isVisible() ) {
        tooltipWindow->show();
      }
      // pozycja nad środkiem kursora
      osgWidget::XYCoord pos = cursor->getOrigin();
      pos.x() += cursor->getWidth() / 2.0;
      pos.y() += cursor->getHeight();
      pos += cursor->getParent()->getAbsoluteOrigin();
      // lewy dolny róg okienka
      pos.x() -= tooltipWindow->getWidth() / 2.0;
      tooltipWindow->setOrigin(pos);
      tooltipWindow->resizeAdd();
      fireEvent(EventIDUpdateTooltip, this, tooltipWindow.get());
    } else {
      if ( tooltipWindow->isVisible() ) {
        tooltipWindow->hide();
      }
    }
  }
}

bool HorizontalScrollBar::onScrollPushed( osgWidget::Event& event )
{
  // zapamiętujemy jaki był początkowy scroll
  draggedValue = currentValue;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////
