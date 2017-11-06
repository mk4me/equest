/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:45
    filename: IConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ICONNECTION_H__
#define HEADER_GUARD___ICONNECTION_H__

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

	//Forward declarations
class IInputPin;
class IOutputPin;

//! Interfejs realizujący połączenie pomiędzy pinami. Wprowadza porzędek pinów poprzez kierunek,
//! pin wyjściowy jest źródłem a pin wejsciowy punktem przeznaczenia
class IConnection
{
public:
    virtual ~IConnection() {}

	//! \return Wezeł źródłowy
	virtual IOutputPin * source() = 0;

	//! \return Wezeł docelowy
	virtual IInputPin * destination() = 0;

	//! \return Wezeł źródłowy
	virtual const IOutputPin * source() const = 0;

	//! \return Wezeł docelowy
	virtual const IInputPin * destination() const = 0;
};

}

#endif	//	HEADER_GUARD___ICONNECTION_H__
