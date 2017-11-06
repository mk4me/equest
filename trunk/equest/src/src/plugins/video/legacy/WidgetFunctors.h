/********************************************************************
	created:  2010/06/11
	created:  11:6:2010   11:25
	filename: WidgetFunctors.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__WIDGETFUNCTORS_H__
#define __HEADER_GUARD__WIDGETFUNCTORS_H__

#include <osg/Drawable>
#include <osgWidget/Widget>
#include <osgWidget/Label>
#include <osgWidget/Window>
#include <osgWidget/EventInterface>


class WidgetColorer : public osg::Drawable::UpdateCallback
{
public:
  //! Bieżący kolor.
  osgWidget::Color current;
  //! Kolor normalny.
  osgWidget::Color normal;
  //! Kolor przy najechaniu.
  osgWidget::Color hoover;
  //! Kolor podczas kliknięcia.
  osgWidget::Color clicked;

  //! Kontruktor z jawnie podanymi parametrami.
  //! \param normal Normalny kolor.
  //! \param hoover Kolor przy najechaniu.
  //! \param clicked Kolor przy kliknięciu.
  WidgetColorer(osgWidget::Color normal, osgWidget::Color hoover, osgWidget::Color clicked)
    : normal(normal), hoover(hoover), clicked(clicked), current(normal)
  {}

  //! Konstruktor domyślny.
  WidgetColorer()
    : normal(osgWidget::Color(0.5f, 0.5f, 0.5f, 1.0f)), hoover(osgWidget::Color(0.75f, 0.75f, 0.75f, 1.0f)),
    clicked(osgWidget::Color(1.0f, 1.0f, 1.0f, 1.0f)), current(normal)
  {}

  //!
  virtual ~WidgetColorer()
  {}

  //! Pomocnicza funkcja ustawiająca kolor.
  //! \return false (po to, aby następujące callbacki tego samego eventa były wywoływane)
  bool setColor(osgWidget::Color color)
  {
    current = color;
    return false;
  }
  //! Najechanie na guzik.
  bool onEnter(osgWidget::Event&)
  {
    return setColor(hoover);
  }
  //! Opuszczenie guzika.
  bool onLeave(osgWidget::Event&)
  {
    return setColor(normal);
  }
  //! Kliknięcie.
  bool onClick(osgWidget::Event&)
  {
    return setColor(clicked);
  }
  //! Zwolnienie kliknięcia.
  bool onRelease(osgWidget::Event&)
  {
    return setColor(hoover);
  }
  //! Aktualizacja.
  virtual void update(osg::NodeVisitor* nv , osg::Drawable* geom)
  {
    update(dynamic_cast<osgWidget::Widget*>(geom));
  }
  //! Konkretna wersja aktualizacji.
  virtual void update(osgWidget::Widget* w)
  {
    w->setColor(current);
  }
  //!
  static void attach(osgWidget::Widget * widget, WidgetColorer * colorer, unsigned int eventMask = osgWidget::EVENT_MASK_MOUSE_DRAG)
  {
    colorer->attachTo(widget, eventMask);
  }
  //!
  virtual void attachTo(osgWidget::Widget * widget, unsigned int eventMask = osgWidget::EVENT_MASK_MOUSE_DRAG)
  {
    widget->setEventMask(eventMask);
    widget->addCallback(new osgWidget::Callback(&WidgetColorer::onEnter, this, osgWidget::EVENT_MOUSE_ENTER));
    widget->addCallback(new osgWidget::Callback(&WidgetColorer::onLeave, this, osgWidget::EVENT_MOUSE_LEAVE));
    widget->addCallback(new osgWidget::Callback(&WidgetColorer::onClick, this, osgWidget::EVENT_MOUSE_PUSH));
    widget->addCallback(new osgWidget::Callback(&WidgetColorer::onRelease, this, osgWidget::EVENT_MOUSE_RELEASE));
    widget->setUpdateCallback(this);
  }
};

//------------------------------------------------------------------------------

class WidgetDragger : public WidgetColorer
{
public:
  //!
  float x;

  //! Kontruktor z jawnie podanymi parametrami.
  //! \param normal Normalny kolor.
  //! \param hoover Kolor przy najechaniu.
  //! \param clicked Kolor przy kliknięciu.
  WidgetDragger(osgWidget::Color normal, osgWidget::Color hoover, osgWidget::Color clicked)
    : WidgetColorer(normal, hoover, clicked), x(0.0f)
  {}

  //! Konstruktor domyślny.
  WidgetDragger() : x(0.0f)
  {}

  //!
  virtual ~WidgetDragger()
  {}

  //!
  virtual void attachTo(osgWidget::Widget * widget, unsigned int eventMask = osgWidget::EVENT_MASK_MOUSE_DRAG)
  {
    WidgetColorer::attachTo(widget, eventMask);
    widget->addCallback(new osgWidget::Callback(&WidgetDragger::onDrag, this, osgWidget::EVENT_MOUSE_DRAG));
  }

  //! Zwolnienie kliknięcia.
  bool onDrag(osgWidget::Event& event)
  {
    x += static_cast<float>(event.x);
    x = std::max<float>(0.0f, x);
    return false;
  }

  //! Konkretna wersja aktualizacji.
  virtual void update(osgWidget::Widget* w)
  {
    WidgetColorer::update(w);
    w->setX( x );
    //x = 0;
  }
};

//------------------------------------------------------------------------------

class LabelFunctor : public osg::Drawable::UpdateCallback
{
public:
  //!
  std::string text;

  //! Aktualizacja.
  virtual void update(osg::NodeVisitor* nv , osg::Drawable* geom)
  {
    update(dynamic_cast<osgWidget::Label*>(geom));
  }

  //! Aktualizacja.
  void update(osgWidget::Label* label)
  {
    label->setLabel(text);
  }
};

//------------------------------------------------------------------------------

class PercentWindowResizer : public osg::NodeCallback
{
private:
  float percentWidth;
  float percentHeight;

public:
  PercentWindowResizer(float percentWidth, float percentHeight = 0.0f)
    : percentWidth(percentWidth), percentHeight(percentHeight)
  {}

  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    osgWidget::Window * window = dynamic_cast<osgWidget::Window*>(node);
    window->resizePercent(percentWidth, percentHeight);
    traverse(node, nv);
  }
};

//------------------------------------------------------------------------------


#endif  // __HEADER_GUARD__WIDGETFUNCTORS_H__
