#include "CorePCH.h"
#include "PluginPath.h"
#include "ApplicationCommon.h"
#include "Path.h"

using namespace core;

PluginPath::PluginPath(const std::string & pluginName)
	: pluginPath(getPaths()->getPluginPath()/pluginName),
	pluginTmpPath(getPaths()->getTempPath()/pluginName)
{
	utils::Filesystem::createDirectory(pluginTmpPath);
}

PluginPath::~PluginPath()
{

}

const utils::Filesystem::Path& PluginPath::getUserDataPath() const
{
	return getPaths()->getUserDataPath();
}

const utils::Filesystem::Path& PluginPath::getApplicationDataPath() const
{
	return getPaths()->getApplicationDataPath();
}

const utils::Filesystem::Path& PluginPath::getUserApplicationDataPath() const
{
	return getPaths()->getUserApplicationDataPath();
}

const utils::Filesystem::Path& PluginPath::getResourcesPath() const
{
	return getPaths()->getResourcesPath();
}

const utils::Filesystem::Path& PluginPath::getTempPath() const
{
	return pluginTmpPath;
}

const utils::Filesystem::Path& PluginPath::getPluginPath() const
{
	return pluginPath;
}