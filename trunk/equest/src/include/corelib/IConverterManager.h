/********************************************************************
    created:  2013/03/22
    created:  22:3:2013   10:26
    filename: IConverterManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ICONVERTERMANAGER_H__
#define HEADER_GUARD___ICONVERTERMANAGER_H__

#include <corelib/IConverter.h>
#include <list>
#include <utils/ObjectWrapper.h>

namespace plugin {

//! Interfejs klasy zapewniaj�cej dost�p do zarejestrowanych konwerter�w
class IConverterManager
{
public:
	//! Typ kolekcji konwerter�w
	typedef std::list<IConverterConstPtr> Converters;

public:
	//! Wirtualny destruktor
	virtual ~IConverterManager() {}

	//! \param input Dane wej�ciowe do konwersji
	//! \param output [out] Wyniki konwersji	
	//! \param recursive Czy wyniki maj� by� generowane rekurencyjnie r�wnie� na bazie wynik�w z poprzednich iteracji
	virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, bool recursive = false) const = 0;

	//! \param input Dane wej�ciowe do konwersji
	//! \param output [out] Wyniki konwersji	
	//! \param requestedTypes Oczekiwane typy wyj�ciowe
	//! \param recursive Czy wyniki maj� by� generowane rekurencyjnie r�wnie� na bazie wynik�w z poprzednich iteracji
	virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, const utils::TypeInfoSet & requestedTypes, bool recursive = false) const = 0;

	//! \param converters [out] Wszystkie zarejestrowane konwertery danych domenowych
	virtual void converters(Converters & converters) const = 0;

	//! \param inTypes Typy danych wej�ciowych kt�re chcemy konwertowa�
	//! \param converters [out] Zarejestrowane konwertery danych domenowych kt�re potrafi� operowa� na danym zbiorze i jego podzbiorach
	virtual void convertersForInputs(const utils::TypeInfoList & inTypes, Converters & converters) const = 0;
	//! \param inTypes Typy danych wej�ciowych kt�re chcemy uzyskac po konwersji
	//! \param converters [out] Zarejestrowane konwertery danych domenowych kt�re potrafi� generowa� dane zadanego typu
	virtual void convertersWithOutputs(const utils::TypeInfoSet & outTypes, Converters & converters) const = 0;
};

}

#endif	//	HEADER_GUARD___ICONVERTERMANAGER_H__
