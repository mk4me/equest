/********************************************************************
	created:	2014/08/12
	created:	9:31
	filename: 	BvhParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__BvhParser_H__
#define HEADER_GUARD_KINEMATIC__BvhParser_H__


#include <corelib/IParser.h>
#include <corelib/IDataManagerReader.h>

//! klasa wykorzystuje bibliotekę kinematiclib aby wczytywać pliki asf
class BvhParser : public plugin::ISourceParser
{    
	UNIQUE_ID("D5ABABBF-D3EF-4C7A-A05C-3EF88162D4DC")
	CLASS_DESCRIPTION("Bvh parser", "Bvh parser")
private:
    //! object wrapper z wczytanym plikiem
    utils::ObjectWrapperPtr skeletalModel;

public:
	//! Konstruktor domyślny
	BvhParser();
	//! Destruktor wirtualny
	virtual ~BvhParser();

public:
    //! Parsowanie pliku
    //! \param path scieżka do parsowanego pliku
	virtual void parse(const std::string & source);
	//! tworzy pusty obiekt parsera
	virtual plugin::IParser* create() const;
	//! zwraca wspierane rozszerzenia plików (asf)
	//! \param extensions kolecja, do której trafią wspierane rozszerzenia
	virtual void acceptedExpressions(Expressions & expressions) const;
	//! Zwraca obiekty dostarczone przez parser
	//! \param objects kolekcja z obiektami (set)
	virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
	//! Reset
	virtual void reset();
};


#endif
