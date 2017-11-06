/********************************************************************
	created:  2010/07/14
	created:  14:7:2010   10:40
	filename: Potentiometer.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__POTENTIOMETER_H__
#define __HEADER_GUARD__POTENTIOMETER_H__

#include <osg/Timer>
#include <osgWidget/Widget>

// TODO: przenieść do nawiasów <>
#include "PotentiometerModel.h"
#include "NestedNodeCallback.h"
#include "Callback.h"

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Kontrolka potencjometru. Połączenie widoku i kontrolera.
 *  Jego działanie polega na przesuwaniu współrzędnych tekstury. Wobec tego
 *  musi być do tego celu specjalnie przygotowana: musi składać się z kafelek
 *  (tiles):
 *      +----+-- ... --+----+
 *      |    |         |    |
 *      +----+-- ... --+----+
 *  Żeby efekt obrotu mógł być uzyskany pierwszy i ostatni tile muszą być
 *  jednakowe. Alternatywą jest tekstura bez "zapętlenia" oraz taki dobór
 *  parametrów, aby nie musiało dojść do obrotu wymagającego zapętlonej
 *  tekstury.
 *  Przy obrocie potencjometru o 2*PI (360 stopni) tekstura ustawia się na
 *  ostatnim tile'u.
 */
class Potentiometer : public osgWidget::Widget, public PotentiometerModel,
  public EventSource
{
public:
  //! Lista eventów.
  enum EventID {
    //! Zmieniła się wartość potencjometru.
    EventIDValueChanged = 1
  };

private:
  //! Aktualizator.
  struct Updater : public osg::Drawable::UpdateCallback
  {
    //! Poprzedni timestamp
    float previous;
    //! Konstruktor zerujący.
    Updater() : previous(0.0f) {}
    //! Aktualizacja.
    virtual void update(osg::NodeVisitor* nv , osg::Drawable* geom);
  };
  friend struct Updater;
  //! 
  using osgWidget::Widget::Corner;

private:
  //! Czas rozpoczęcia dotyku.
  osg::Timer pushTime;
  //! Miejsce rozpoczęcia dotyku.
  osgWidget::XYCoord pushDelta;
  //! Szerokość tile'i w jednostkach tekstury.
  osgWidget::texcoord_type textureTileWidth;
  //! Parametr mówiący, jak bardzo długość ruchu będzie przekładana na siłę
  //! przyłożoną do potencjometru.
  /** Przyłożona siła = Wektor wodzący myszy * czułość / (deltaCzasowa * szerokość/2) */
  float sensetivity;
  //! Poprzednia wartość.
  float prevValue;

public:

  META_Object(osgUI, Potentiometer)
  //! 
  //! \param name
  //! \param width
  //! \param height
  Potentiometer(const std::string& name = "", osgWidget::point_type width = 0, 
    osgWidget::point_type height = 0);

  //! Konstruktor kopiujący.
  //! \param src
  //! \param copyop
  Potentiometer(const Potentiometer& src, const osg::CopyOp& copyop);

  //! \return
  inline float getSensetivity() const
  { 
    return sensetivity;
  }
  //! \param sensetivity
  inline void setSensetivity(float sensetivity) 
  { 
    this->sensetivity = sensetivity; 
  }
  //! \return Szerokość tile'i w jednostkach tekstury.
  inline osgWidget::texcoord_type getTextureTileWidth() const
  { 
    return textureTileWidth;
  }
  //! \param textureTileWidth Szerokość tile'i w jednostkach tekstury.
  inline void setTextureTileWidth(osgWidget::texcoord_type textureTileWidth) 
  { 
    this->textureTileWidth = textureTileWidth; 
  }
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mousePush(double x, double y, const osgWidget::WindowManager* wm);
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mouseRelease(double x, double y, const osgWidget::WindowManager* wm);
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mouseLeave(double x, double y, const osgWidget::WindowManager* wm);
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mouseDrag(double x, double y, const osgWidget::WindowManager* wm);

private:
  //! Reakcja na wykonanie ruchu myszą.
  //! \param delta
  void notifyDrag(osgWidget::XYCoord delta);

  //! Ukrycie metody z klasy bazowej.
  //! \param deltaTime Delta.
  void update(double deltaTime);

  //! Pomocnicza funkcja do ustawiania koordynatów tekstury.
  //! \param s
  //! \param corner
  inline void setTexCoordS(osgWidget::texcoord_type s, Corner corner)
  {
    setTexCoord( s, getTexCoord(corner).y(), corner );
  }

  //! Przeładowanie metody z obserwatora.
  virtual void notify();

  //! Ukrycie metody bazowej.
  using PotentiometerModel::setIsPushed;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__POTENTIOMETER_H__
