/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:13
	filename: 	Scales.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__SCALES_H__
#define HEADER_GUARD_NEW_CHART__SCALES_H__

#include <limits>

//! Prosta klasa, ułatwiająca zarządzać ekstremami wykresu
class Scales
{
public:
    //! Konstruktor, zostawia obiekt niezainicjalizowany
    Scales() : 
	  xMin((std::numeric_limits<float>::max)()),
      xMax((std::numeric_limits<float>::min)()),
      yMin((std::numeric_limits<float>::max)()),
      yMax((std::numeric_limits<float>::min)()),
      initialized(false) 
    {}

    //! Konstruktor, inicjalizuje obiekt
    //! \param xMin minimalny argument na wykresie
    //! \param xMax maksymalny argument na wykresie
    //! \param yMin wartość minimalna na wykresie
    //! \param yMax wartość maksymalna na wykresie
    Scales(float xMin, float xMax, float yMin, float yMax) :
      xMin(xMin),
      xMax(xMax),
      yMin(yMin),
      yMax(yMax),
      initialized(true) 
    {}

public:
    //! \return wartość maksymalna na wykresie
    float getYMax() const { return yMax; }
    //! \return wartość minimalna na wykresie
    float getYMin() const { return yMin; }
    //! \return maksymalny argument na wykresie
    float getXMax() const { return xMax; }
    //! \return minimalny argument na wykresie
    float getXMin() const { return xMin; }
    //! \return czy obiekt już został zainicjalizowany poprawnymi danymi
    bool isInitialized() const { return initialized; }

public:
    //! resetowanie ustawien, zostawia obiekt niezainicjalizowany
    void clear() { initialized = false; }
    //! łączy obiekt z drugim obiektem tego samego typu, w razie potrzeby zmieniane są ekstrema
    //! \param scales drugi obiekt z ekstremami
    void merge(const Scales& scales);

private:
    //! minimalny argument na wykresie
    float xMin;
    //! maksymalny argument na wykresie
    float xMax;
    //! wartość minimalna na wykresie
    float yMin;
    //! wartość maksymalna na wykresie
    float yMax;
    //! flaga okresla, czy obiekt już został zainicjalizowany poprawnymi danymi
    bool initialized;
};



#endif
