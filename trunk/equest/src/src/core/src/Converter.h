/********************************************************************
    created:  2013/03/25
    created:  25:3:2013   15:33
    filename: Converter.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CONVERTER_H__
#define HEADER_GUARD___CONVERTER_H__

#include <corelib/IConverter.h>

namespace core {

	class Converter
	{
	public:
		Converter(const plugin::IConverterPtr & converter);
		~Converter();

		const int indexForOutputType(const utils::TypeInfo & type) const;
		
	};

}

#endif	//	HEADER_GUARD___CONVERTER_H__
