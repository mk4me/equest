/********************************************************************
    created:  2013/01/11
    created:  11:1:2013   8:18
    filename: ParserManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PARSERMANAGER_H__
#define HEADER_GUARD___PARSERMANAGER_H__

#include <corelib/IParserManagerReader.h>
#include <regex>

namespace core {

class ParserManager : public IParserManagerReader
{
private:

	//! Struktura rozszerzająca opis wyrażenia o wyrażenie regularne uzywane do weryfikacji ścieżki źródła
	struct ExpressionDescription : public plugin::IParser::ExpressionDescription
	{
		std::regex regularExpression;		//! Realizacja wyrażenia regularnego
		utils::TypeInfoSet supportedTypes;	//! Zbiór typów wspieranych, zarejestrowanych w HierarchyManager
		TypesMap typesMap;					//! Mapa indeksów wspieranych typów w HierarchyManager jakie parser oferuje dla danego źródła
	};

	//! Mapa 
	typedef std::map<std::string, ExpressionDescription> Expressions;

	//! Dane wyciągnięte z parsera, używamy ich do określenia czy dane zasoby mogą być przez parser obsłużone
	//! Opis parsera
	struct ParserData
	{
		//! Parser
		plugin::IParserPtr parser;
		//! Obsugiwane wyrażenia
		Expressions expressions;
		//! Możliwości parsera
		//! Strumienie
		plugin::IStreamParser * streamParser;
		//! Własne IO
		plugin::ISourceParser * sourceParser;
	};

	//! Zbiór parserów z ich opisami
	typedef std::map<core::UniqueID, ParserData> Parsers;

private:
	//! Zarejestrowane parsery
	Parsers parsers_;

private:

	//! \param source Źródło które weryfikujemy
	//! \param expression Wyrażenie względem którego weryfikujemy
	//! \return Czy źródło pasuje do wyrażenia
	static const bool verifyExpression(const std::string & source,
		const std::regex & expression);

	//! \param source Źródło danych które weryfikujemy	
	//! \param expressions Wyrażenia względem których weryfikujemy źródło
	//! \return Czy źródło pasuje do któregoś z wyrażeń
	static const bool verifyParserExpressions(const std::string & source,		
		const ParserManager::Expressions & expressions);

public:
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów
	virtual void parsers(ParserPrototypes & parserPrototypes) const;
	//! \param source Ścieżka źródła danych
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów akceptującymi zadane źródło
	virtual void parsers(const std::string & source, ParserPrototypes & parserPrototypes) const;
	//! Tylko parsery z włąsnym I/O
	//! \param source Ścieżka źródła danych
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów akceptującymi zadane źródło
	virtual void sourceParsers(const std::string & source, ParserPrototypes & parserPrototypes) const;
	//! Tylko parsery strumieniowe
	//! \param source Ścieżka źródła danych
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów akceptującymi zadane źródło
	virtual void streamParsers(const std::string & source, ParserPrototypes & parserPrototypes) const;
	//! \param source Ścieżka źródła danych dla której spradzamy czy są jakieś parsery
	//! \return Prawda jeśli dana ścieżka źródła jest obsługiwana przez jakiś parser
	virtual const bool sourceIsAccepted(const std::string & source) const;
	//! \param source Ścieżka źródła danych dla której spradzamy możliwe do uzyskania typy danych
	//! \param types [out] Zbiór możliwych do uzyskania z danego źródła typów danych
	virtual void sourcePossibleTypes(const std::string & source, utils::TypeInfoSet & types) const;
	//! \param parserID Identyfikator parsera
	//! \param source Źródło
	//! \return Wspierane typy zarejestrowane w HierarchyManager
	virtual const TypesMap parserTypes(const UniqueID parserID, const std::string & source) const;
	//! \param parser Rejestrowany parser
	void registerParser(const plugin::IParserPtr & parser);
};

}

#endif	//	HEADER_GUARD___PARSERMANAGER_H__
