/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:51
    filename: IDFNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDFNODE_H__
#define HEADER_GUARD___IDFNODE_H__

#include <dflib/IDFFeatures.h>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

//! Aby stworzyć źródło danych, sink czy procesor należy rozszerzyć implementację podstawowych
//! interfejsów węzłów o odpowiednie interfejsy znajdujące się poniżej.

//! Interfejs wymagany przez źródła danych
class IDFSource : public IDFResetable
{
public:
	//! \return Czy źródło ma jeszcze dane do wystawienia
	virtual const bool empty() const = 0;
	//! Metoda produkująca/wystawiająca dane w źródle
	virtual void produce() = 0;
};

//! Interfejs wymagany przez sinki danych
class IDFSink : public IDFResetable
{
public:

	//! Metoda obsługująca dane wejsciowe ( najczęściej jakieś I/O, serializacja)
	virtual void consume() = 0;
};

//! Interfejs wymagany przez procesory
class IDFProcessor : public IDFResetable
{
public:

	//! Metoda przetwarzająca dane w elemencie przetwarzającym (mamy wszystkie dane wejsciowe gotowe, z naszych wyjść wszystko zostało już odczytane)
	virtual void process() = 0;
};

}

#endif	//	HEADER_GUARD___IDFNODE_H__
