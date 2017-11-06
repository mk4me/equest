#include "CorePCH.h"
#include "ApplicationDescription.h"
#include <QtCore/QCoreApplication>

using namespace core;

ApplicationDescription::ApplicationDescription(const ExtendedVersion * version,
	const plugin::IVendorDescription * vendor)
	: version_(version), vendor_(vendor)
{

}

ApplicationDescription::~ApplicationDescription()
{

}

const ExtendedVersion * ApplicationDescription::version() const
{
	return version_;
}

const std::string ApplicationDescription::name() const
{
	return qApp->applicationName().toStdString();
}

const std::string ApplicationDescription::shortName() const
{
	return shortName_;
}

const std::string ApplicationDescription::description() const
{
	return description_;
}

const plugin::IVendorDescription * ApplicationDescription::vendor() const
{
	return vendor_;
}

void ApplicationDescription::setName(const std::string & name)
{
	qApp->setApplicationName(QString::fromStdString(name));
}

void ApplicationDescription::setShortName(const std::string & shortName)
{
	shortName_ = shortName;
}

void ApplicationDescription::setDescription(const std::string & description)
{
	description_ = description;
}