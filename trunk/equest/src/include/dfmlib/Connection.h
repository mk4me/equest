#ifndef HEADER_GUARD__CONNECTION_H__
#define HEADER_GUARD__CONNECTION_H__

#include <string>
#include <dfmlib/DFLMTypes.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////


//! Klasa opisująca połączenie dwóch pinów. Można z niej pobrać piny w uporządkowanej kolejności - pin wyjściowy (źródło) - pin wejściowy (odbiorca).
class Connection
{

public:

    //! Klasa zaprzyjaźniona - model ma pełną kontrolę nad połączeniami - sam je tworzy i nimi zarządza, użytkownik tylko z nich korzysta
	friend class Model;

public:

    //! Konstruktor chroniony, pozwala dziedziczyć ale nie pozwala klientowi samemu tworzyć połączenia i ich modyfikować
    //! \param src Węzeł źródłowy
    //! \param dest Węzeł docelowy
    Connection(const PinPtr & src, const PinPtr & dest);

    //! Wirtualny destruktor
	virtual ~Connection(void);

    //! \return Wezeł źródłowy
	PinPtr getSrc() const;

    //! \return Wezeł docelowy
	PinPtr getDest() const;

    //! \return drugi węzeł biorący udział w połączeniu lub wyjątek jeśli węzeł nie bierze udziału w tym połączeniu
	PinPtr getOther(const CWPinPtr & pin) const;

    //! \return Nazwa połaczenia
	std::string getConnectionName() const;

private:
    //! \param src Węzeł źródłowy(wyjściowy)
	void setSrc(const PinPtr & src);

    //! \param dest Węzeł docelowy(wejściowy)
	void setDest(const PinPtr & dest);

private:
    //! Węzeł źródłowy połączenia
	WPinPtr srcPin;

    //! Węzeł docelowy połaczenia
	WPinPtr destPin;
};

}

#endif
