/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:54
    filename: IDFPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDFPIN_H__
#define HEADER_GUARD___IDFPIN_H__

#include <dflib/IPin.h>
#include <dflib/IDFFeatures.h>

////////////////////////////////////////////////////////////////////////////////
namespace df {
////////////////////////////////////////////////////////////////////////////////

//! Zbiór interfejsów na potrzeby przetwarzania danych - przekazywania danych
//! miêdzy wêz³ami przez piny. Aby stworzyæ pin z danymi œwiadomy przetwarzania danych
//! i ich przekazywania nale¿y rozszerzyæ implementacjê podstaowych interfejsów pinów
//! o odpowiednie interfejsy zamieszczone poni¿ej

//! Interfejs pinu wyjœciowego œwiadomego danych
class IDFOutput : public IDFResetable
{

};

//! Interfejs pinu wejœciowego œwiadomego danych
class IDFInput : public IDFResetable
{
public:
	//! \param pin Wyjœciowy pin z którego kopiujemy dane
	virtual void copyData(const IDFOutput * pin) = 0;
};


}

#endif	//	HEADER_GUARD___IDFPIN_H__
