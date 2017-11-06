/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:29
    filename: Connection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CONNECTION_H__
#define HEADER_GUARD___CONNECTION_H__

#include <dflib/IConnection.h>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

//! Klasa opisująca połączenie dwóch pinów. Można z niej pobrać piny w uporządkowanej kolejności - pin wyjściowy (źródło) - pin wejściowy (odbiorca).
class Connection : public IConnection
{
public:

    //! Konstruktor
    //! \param src Węzeł źródłowy
    //! \param dest Węzeł docelowy
    Connection(IOutputPin * src, IInputPin * dest);

    //! Wirtualny destruktor
	virtual ~Connection(void);

	//! \return Wezeł źródłowy
	virtual IOutputPin * source();

	//! \return Wezeł docelowy
	virtual IInputPin * destination();

	//! \return Wezeł źródłowy
	virtual const IOutputPin * source() const;

	//! \return Wezeł docelowy
	virtual const IInputPin * destination() const;

private:
    //! Węzeł źródłowy połączenia
	IOutputPin * source_;

    //! Węzeł docelowy połaczenia
	IInputPin * destination_;
};

}

#endif	//	HEADER_GUARD___CONNECTION_H__
