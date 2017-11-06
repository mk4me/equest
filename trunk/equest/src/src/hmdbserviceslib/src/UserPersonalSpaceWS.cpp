#include <hmdbserviceslib/UserPersonalSpaceWS.h>

namespace hmdbServices
{
	UserPersonalSpaceWS::UserPersonalSpaceWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	UserPersonalSpaceWS::~UserPersonalSpaceWS()
	{
	}
}