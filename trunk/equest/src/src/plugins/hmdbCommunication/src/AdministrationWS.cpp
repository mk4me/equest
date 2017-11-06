#include "AdministrationWS.h"
#include <hmdbserviceslib/AdministrationWS.h>

using namespace hmdbCommunication;

AdministrationWS::AdministrationWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

AdministrationWS::~AdministrationWS()
{
}

void AdministrationWS::defineAttributeGroup(const std::string & groupName,
	const std::string & entity)
{
	hmdbServices::AdministrationWS(creator()).defineAttributeGroup(groupName,
		entity);
}

void AdministrationWS::removeAttributeGroup(const std::string & groupName,
	const std::string & entity)
{
	hmdbServices::AdministrationWS(creator()).removeAttributeGroup(groupName,
		entity);
}

void AdministrationWS::defineAttribute(const std::string & attributeName,
	const std::string & groupName, const std::string & entity, bool isEnum,
	const std::string & pluginDescriptor, const std::string & type,
	const std::string & unit)
{
	hmdbServices::AdministrationWS(creator()).defineAttribute(attributeName,
		groupName, entity, isEnum, pluginDescriptor, type, unit);
}

void AdministrationWS::removeAttribute(const std::string & attributeName,
	const std::string & groupName, const std::string & entity)
{
	hmdbServices::AdministrationWS(creator()).removeAttribute(attributeName,
		groupName, entity);
}

void AdministrationWS::addAttributeEnumValue(const std::string & attributeName,
	const std::string & groupName, const std::string & entity,
	const std::string & value, const bool clearExisting)
{
	hmdbServices::AdministrationWS(creator()).addAttributeEnumValue(attributeName,
		groupName, entity, value, clearExisting);
}

void AdministrationWS::downloadAreaCleanup(const int olderThanMinutes)
{
	hmdbServices::AdministrationWS(creator()).downloadAreaCleanup(olderThanMinutes);
}
