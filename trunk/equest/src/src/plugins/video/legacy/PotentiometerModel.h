/********************************************************************
	created:  2010/07/12
	created:  12:7:2010   14:27
	filename: PotentiometerModel.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__POTENTIOMETERMODEL_H__
#define __HEADER_GUARD__POTENTIOMETERMODEL_H__

#include <utils/ObserverPattern.h>

struct PotentiometerModelData
{
  //! Moment bezwładności. Zastępuje promień i masę.
  /** Moment bezwładności walca to 0.5 * m * r^2 */
  float momentOfInteria;
  //! Ostatnia delta kątu obrotu. W radianach.
  float delta;
  //! Przyłożona siła.
  float forcedMoment;
  //! Moment tarcia powietrza.
  float airFriction;
  //! Moment tarcia dotyku.
  float clickFriction;
  //! Prędkość kątowa.
  float angularVelocity;
  //! Czy jest dotykany?
  bool pushed;
  //! Maksymalna wartość obrotu (radiany).
  float maxValue;
  //! Minimalna wartość obrotu (radiany).
  float minValue;
  //! Bieżąca wartość (radiany).
  float value;
};

/**
 *	
 */
class PotentiometerModel : public utils::Observable<PotentiometerModel>, private PotentiometerModelData
{
public:
  //! Konstruktor zerujący.
  PotentiometerModel();
  //! Konstruktor kopiujący.
  PotentiometerModel(const PotentiometerModel& src);

  //! Aktualizacja modelu fizycznego.
  //! \param deltaTime Ile czasu upłynęło.
  void update(float deltaTime);

  //! \return Delta obrotu (radiany).
  inline float getDelta() const
  { 
    return delta;
  }
  //! \return
  inline float getMoment() const
  { 
    return forcedMoment;
  }
  //! \param moment
  inline void setMoment(float moment) 
  { 
    this->forcedMoment = moment; 
    notify();
  }
  //! Przykłada siłę (impuls) do modelu.
  /**
   *	Stosujemy uproszczony model. Ponieważ zakładadamy, że potencjometr ma promień równy 1
   *  oraz siłe przykłada się zawsze do zewnętrzych ścianek wektorem prostopadłym do osi symetrii
   *  forcedMoment obrotowy staje się równy sumie przyłożonych sił.
   *  \param magnitude Długość wektora siły.
   */
  inline void addMoment(float magnitude)
  {
    this->forcedMoment += magnitude;
    notify();
  }
  //! \return
  inline bool isPushed() const
  { 
    return pushed;
  }
  //! \param isTouched
  inline void setIsPushed(bool pushed) 
  { 
    this->pushed = pushed; 
    notify();
  }
  //! \return
  inline float getMomentOfInteria() const
  { 
    return momentOfInteria;
  }
  //! \param momentOfInteria
  inline void setMomentOfInteria(float momentOfInteria) 
  { 
    this->momentOfInteria = momentOfInteria; 
    notify();
  }
  //! \return
  inline float getAirFriction() const
  { 
    return airFriction;
  }
  //! \param airFriction
  inline void setAirFriction(float airFriction) 
  { 
    this->airFriction = airFriction; 
    notify();
  }
  //! \return
  inline float getClickFriction() const
  { 
    return clickFriction;
  }
  //! \param clickFriction
  inline void setClickFriction(float clickFriction) 
  { 
    this->clickFriction = clickFriction; 
    notify();
  }
  //! \return
  inline float getMaxValue() const
  { 
    return maxValue;
  }
  //! \param maxValue
  inline void setMaxValue(float maxValue) 
  { 
    this->maxValue = maxValue; 
    notify();
  }
  //! \return
  inline float getValue() const
  { 
    return value;
  }
  //! Do ustawienia wartości z zewnąrz. Zeruje model fizyczny.
  //! \param value
  inline void setValue(float value) 
  { 
    delta = value - this->value;
    this->value = value;
    notify();
  }
  //! \return
  inline float getMinValue() const
  { 
    return minValue;
  }
  //! \param minValue
  inline void setMinValue(float minValue) 
  { 
    this->minValue = minValue; 
    notify();
  }
  //! \return
  inline float getNormalizedValue() const
  { 
    return (value - minValue) / (maxValue - minValue);
  }
  //! \param value
  inline void setNormalizedValue(float value) 
  { 
    setValue((1 - value) * minValue + value * maxValue);
  }
  //! Zeruje fizykę.
  inline void zeroPhysics()
  {
    angularVelocity = 0.0f;
  }
};


#endif  // __HEADER_GUARD__POTENTIOMETERMODEL_H__
