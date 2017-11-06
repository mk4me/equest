/********************************************************************
    created:  2014/04/02
    created:  2:4:2014   10:08
    filename: IApplicationDescription.h
    author:   Mateusz Janiak
    
    purpose:  Klasa dostarczaj¹ca informacji o aplikacji
*********************************************************************/
#ifndef HEADER_GUARD___IAPPLICATIONDESCRIPTION_H__
#define HEADER_GUARD___IAPPLICATIONDESCRIPTION_H__

#include <corelib/IPluginDescription.h>

namespace core
{
	class IApplicationDescription : public plugin::IPluginDescription
	{
	public:
		//! \return Wersja aplikacji
		virtual const ExtendedVersion * version() const = 0;
	};

}

#endif	//	HEADER_GUARD___IAPPLICATIONDESCRIPTION_H__
