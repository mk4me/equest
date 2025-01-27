/********************************************************************
    created:  2013/03/22
    created:  22:3:2013   9:58
    filename: IConverter.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/

#ifndef HEADER_GUARD___ICONVERTER_H__
#define HEADER_GUARD___ICONVERTER_H__

#include <corelib/IIdentifiable.h>
//#include <utils/SmartPtr.h>
#include <corelib/BaseDataTypes.h>
#include <set>

namespace plugin {

	class IConverter
	{

	};

	//! Interfejs obiektów konwertujących obiekty domenowe
	/*class IConverter : public IIdentifiable, public IDescription
	{
	public:
		//! Typ maski dla danych wyjściowych
		typedef std::set<utils::TypeInfoVector::size_type> OutputMask;

	public:
		//! Wirtualny destruktor
		virtual ~IConverter() {}

		//! \param inTypes [out] Wektor typów wraz z ich kolejnością wymaganych do konwersji
		//! \param outTypes [out] Wektor typów wraz z ich kolejnością jakie powstaną po konwersji
		virtual void description(utils::TypeInfoVector & inTypes, utils::TypeInfoVector & outTypes) const = 0;

		//! \param input Dane wejściowe w formie obiektów domenowych opakowanych w ObjectWrapperPtr
		//! \param output [out] Dane wyjściowe - wszystkie możliwe do wygenerowania
		virtual void convert(const utils::ConstObjectsVector & input, utils::ConstObjectsList & output) const = 0;
		
		//! \param input Dane wejściowe w formie obiektów domenowych opakowanych w ObjectWrapperPtr
		//! \param output [out] Dane wyjściowe zgodnie z żądaniem
		//! \param outputMask Maska dla danych jakie mają być wygenerowane (nakłądana an wektor opisu wyjścia dla description)
		virtual void convert(const utils::ConstObjectsVector & input, utils::ConstObjectsList & output, const OutputMask & outputMask) const = 0;
	};*/

	typedef core::shared_ptr<IConverter> IConverterPtr;
	typedef core::shared_ptr<const IConverter> IConverterConstPtr;
	typedef core::weak_ptr<IConverter> IConverterWeakPtr;
}

#endif	//	HEADER_GUARD___ICONVERTER_H__
