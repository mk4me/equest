/********************************************************************
	created:  2012/03/02
	created:  2:3:2012   14:30
	filename: AdministrationWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBCOMMUNICATION_ADMINISTRATIONWS_H__
#define HEADER_GUARD__HMDBCOMMUNICATION_ADMINISTRATIONWS_H__

#include "WSDLServiceTypes.h"
#include <hmdbserviceslib/IAdministrationWS.h>

namespace hmdbCommunication
{
	class AdministrationWS : public hmdbServices::IAdministrationWS
	{
	public:

		AdministrationWS(const WSDLServiceCreator creator);
		virtual ~AdministrationWS();

		virtual void defineAttributeGroup(const std::string & groupName, const std::string & entity);

		virtual void removeAttributeGroup(const std::string & groupName, const std::string & entity);

		virtual void defineAttribute(const std::string & attributeName, const std::string & groupName,
			const std::string & entity, const bool isEnum, const std::string & pluginDescriptor,
			const std::string & type, const std::string & unit);

		virtual void removeAttribute(const std::string & attributeName, const std::string & groupName,
			const std::string & entity);

		virtual void addAttributeEnumValue(const std::string & attributeName, const std::string & groupName,
			const std::string & entity, const std::string & value, const bool clearExisting);

		virtual void downloadAreaCleanup(const int olderThanMinutes);

	private:
		//! Serwis obs³uguj¹cy t¹ us³ugê
		const WSDLServiceCreator creator;
	};
}

#endif	//	HEADER_GUARD__HMDBCOMMUNICATION_ADMINISTRATIONWS_H__
