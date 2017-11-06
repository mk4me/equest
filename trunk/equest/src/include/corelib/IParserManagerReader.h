/********************************************************************
    created:  2013/01/14
    created:  14:1:2013   16:13
    filename: IParserManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPARSERMANAGERREADER_H__
#define HEADER_GUARD___IPARSERMANAGERREADER_H__

#include <list>
#include <map>
#include <corelib/IParser.h>

namespace core {

	//! Interfejs dajπcy dostÍp do zarejestrowanych w aplikacji parserÛw
	class IParserManagerReader
	{
	public:
		//! Typ agregujπcy prototypy parserÛw
		typedef std::list<plugin::IParserConstPtr> ParserPrototypes;
		//! Typ opisujπcy indexy i typ parsera dla danego ürÛd≥a
		typedef std::map<int, utils::TypeInfo> TypesMap;

	public:
		//! Destrutkor wirtualny
		virtual ~IParserManagerReader() {}

		//! \param parserPrototypes [out] Kolekcja wype≥niania aktualnie znanymi prototypami parserÛw
		virtual void parsers(ParserPrototypes & parserPrototypes) const = 0;
		//! \param source åcieøka ürÛd≥a danych
		//! \param parserPrototypes [out] Kolekcja wype≥niania aktualnie znanymi prototypami parserÛw akceptujπcymi zadane ürÛd≥o
		virtual void parsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! Tylko parsery z w≥πsnym I/O
		//! \param source åcieøka ürÛd≥a danych
		//! \param parserPrototypes [out] Kolekcja wype≥niania aktualnie znanymi prototypami parserÛw akceptujπcymi zadane ürÛd≥o
		virtual void sourceParsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! Tylko parsery strumieniowe
		//! \param source åcieøka ürÛd≥a danych
		//! \param parserPrototypes [out] Kolekcja wype≥niania aktualnie znanymi prototypami parserÛw akceptujπcymi zadane ürÛd≥o
		virtual void streamParsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! \param source åcieøka ürÛd≥a danych dla ktÛrej spradzamy czy sπ jakieú parsery
		//! \return Prawda jeúli dana úcieøka ürÛd≥a jest obs≥ugiwana przez jakiú parser
		virtual const bool sourceIsAccepted(const std::string & source) const = 0;
		//! \param source åcieøka ürÛd≥a danych dla ktÛrej spradzamy moøliwe do uzyskania typy danych
		//! \param types [out] ZbiÛr moøliwych do uzyskania z danego ürÛd≥a typÛw danych
		virtual void sourcePossibleTypes(const std::string & source, utils::TypeInfoSet & types) const = 0;
		//! \param parserID Identyfikator parsera
		//! \param source èrÛd≥o
		//! \return Wspierane typy zarejestrowane w HierarchyManager
		virtual const TypesMap parserTypes(const core::UniqueID parserID, const std::string & source) const = 0;
	};
}

#endif	//	HEADER_GUARD___IPARSERMANAGERREADER_H__