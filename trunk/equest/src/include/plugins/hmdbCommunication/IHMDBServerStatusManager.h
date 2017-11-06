/********************************************************************
	created:  2014/07/11	19:08:35
	filename: IHMDBServerStatusManager.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVERSTATUSMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVERSTATUSMANAGER_H__

#include <corelib/Filesystem.h>

namespace hmdbCommunication
{
	class IHMDBServerStatusManager
	{
	public:
		//! Struktura opisuj¹ca status pingu
		struct ServerStatus
		{
			//! Adres do pingowania
			std::string serverUrl;
			//! Status serwera
			bool online;
			//! B³¹d pingowania jeœli by³
			std::string error;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBServerStatusManager() {}
		//! \param url Adres serwera do pingowania
		//! \param caPath Œcie¿ka ewentualnego certyfikatu SSL je¿eli wymagany
		//! \return Status serwera
		virtual const ServerStatus serverStatus(const std::string & url, const core::Filesystem::Path & caPath = core::Filesystem::Path()) const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVERSTATUSMANAGER_H__