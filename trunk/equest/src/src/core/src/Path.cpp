#include "CorePCH.h"
#include "Path.h"

using namespace core;

Path::Path(const utils::Filesystem::Path & userDataPath,
	const utils::Filesystem::Path & applicationDataPath,
	const utils::Filesystem::Path & userApplicationDataPath,
	const utils::Filesystem::Path & resourcesPath,
	const utils::Filesystem::Path & tempPath,
	const utils::Filesystem::Path & pluginPath)
	: translationsPath_(resourcesPath / "lang"),
	  userDataPath_(userDataPath),
	  applicationDataPath_(applicationDataPath),
	  userApplicationDataPath_(userApplicationDataPath),
	  resourcesPath_(resourcesPath),
	  tempPath_(tempPath),
	  pluginPath_(pluginPath)
{
	translationsPath_.make_preferred();
	userDataPath_.make_preferred();
	applicationDataPath_.make_preferred();
	userApplicationDataPath_.make_preferred();
	resourcesPath_.make_preferred();
	tempPath_.make_preferred();
	pluginPath_.make_preferred();	
}

Path::~Path()
{

}

const utils::Filesystem::Path& Path::getUserDataPath() const
{
	return userDataPath_;
}

const utils::Filesystem::Path& Path::getApplicationDataPath() const
{
	return applicationDataPath_;
}


const utils::Filesystem::Path& Path::getUserApplicationDataPath() const
{
	return userApplicationDataPath_;
}

const utils::Filesystem::Path& Path::getResourcesPath() const
{
	return resourcesPath_;
}

const utils::Filesystem::Path& Path::getTempPath() const
{
	return tempPath_;
}

const utils::Filesystem::Path& Path::getPluginPath() const
{
	return pluginPath_;
}

const utils::Filesystem::Path& Path::getTranslationsPath() const
{
	return translationsPath_;
}