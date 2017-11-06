/********************************************************************
	created:  2012/03/02
	created:  2:3:2012   14:38
	filename: UserPersonalSpaceWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_USERPERSONALSPACEWS_H__
#define HEADER_GUARD__HMDBSERVICES_USERPERSONALSPACEWS_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/IUserPersonalSpaceWS.h>
#include <networkUtils/IWSDLService.h>

namespace hmdbServices
{
	class HMDBSERVICES_EXPORT UserPersonalSpaceWS : public IUserPersonalSpaceWS
	{
	public:
		UserPersonalSpaceWS(networkUtils::IWSDLServicePtr wsdlService);
		virtual ~UserPersonalSpaceWS();

	private:
		//! Serwis obs³uguj¹cy t¹ us³ugê
		networkUtils::IWSDLServicePtr wsdlService;
	};
}

#endif	//	HEADER_GUARD__HMDBSERVICES_USERPERSONALSPACEWS_H__
