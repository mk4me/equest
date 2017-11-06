/********************************************************************
    created:  2014/04/02
    created:  2:4:2014   11:39
    filename: IPluginDescription.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs opisuj¹cy plugin
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN__IPLUGINDESCRIPTION_H__
#define HEADER_GUARD_PLUGIN__IPLUGINDESCRIPTION_H__

#include <corelib/IIdentifiable.h>
#include <corelib/Version.h>

namespace plugin
{
	class IPluginDescription : public IDescription
	{
	public:
		//! Destruktor wirtualny
		virtual ~IPluginDescription() {}
		//! \return Wersja pluginu
		virtual const core::Version * version() const = 0;
		//! \return Dostawca pluginu
		virtual const IVendorDescription * vendor() const = 0;
	};
}

#endif	//	HEADER_GUARD_PLUGIN__IPLUGINDESCRIPTION_H__
