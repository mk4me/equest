#include <hmdbserviceslib/AdministrationWS.h>

namespace hmdbServices
{
	AdministrationWS::AdministrationWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	AdministrationWS::~AdministrationWS()
	{
	}

	void AdministrationWS::defineAttributeGroup(const std::string & groupName,
		const std::string & entity)
	{
		wsdlService->setOperation("DefineAttributeGroup");
		wsdlService->setValue("groupName", groupName);
		wsdlService->setValue("entity", entity);
		wsdlService->invoke();
	}

	void AdministrationWS::removeAttributeGroup(const std::string & groupName,
		const std::string & entity)
	{
		wsdlService->setOperation("RemoveAttributeGroup");
		wsdlService->setValue("groupName", groupName);
		wsdlService->setValue("entity", entity);
		wsdlService->invoke();
	}

	void AdministrationWS::defineAttribute(const std::string & attributeName,
		const std::string & groupName, const std::string & entity, bool isEnum,
		const std::string & pluginDescriptor, const std::string & type,
		const std::string & unit)
	{
		wsdlService->setOperation("DefineAttribute");
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("groupName", groupName);
		wsdlService->setValue("entity", entity);
		wsdlService->setValue("isEnum", isEnum);
		wsdlService->setValue("pluginDescriptor", pluginDescriptor);
		wsdlService->setValue("type", type);
		wsdlService->setValue("unit", unit);
		wsdlService->invoke();
	}

	void AdministrationWS::removeAttribute(const std::string & attributeName,
		const std::string & groupName, const std::string & entity)
	{
		wsdlService->setOperation("RemoveAttribute");
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("entity", entity);
		wsdlService->invoke();
	}

	void AdministrationWS::addAttributeEnumValue(const std::string & attributeName,
		const std::string & groupName, const std::string & entity,
		const std::string & value, const bool clearExisting)
	{
		wsdlService->setOperation("AddAttributeEnumValue");
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("groupName", groupName);
		wsdlService->setValue("entity", entity);
		wsdlService->setValue("value", value);
		wsdlService->setValue("clearExisting", clearExisting);
		wsdlService->invoke();
	}

	void AdministrationWS::downloadAreaCleanup(const int olderThanMinutes)
	{
		wsdlService->setOperation("DownloadAreaCleanup");
		wsdlService->setValue("olderThanMinutes", olderThanMinutes);
		wsdlService->invoke();
	}
}