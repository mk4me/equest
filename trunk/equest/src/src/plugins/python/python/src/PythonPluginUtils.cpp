#include "PythonPCH.h"
#include "corelib/PluginCommon.h"
#include "corelib/IServiceManager.h"
#include "corelib/ISourceManager.h"
#include "plugins/python/python/PythonPluginUtils.h"
#include "PythonService.h"

core::IServiceManager* python::PythonPluginUtils::getServiceManager()
{
	return plugin::getServiceManager();
}

core::ISourceManager* python::PythonPluginUtils::getSourceManager()
{
	return plugin::getSourceManager();
}

core::IDataHierarchyManager* python::PythonPluginUtils::getHierarchyManager()
{
	//return plugin::getDataHierarchyManagerReader();
	auto pythonService = core::queryService<PythonService>(plugin::getServiceManager());
	return pythonService->getHierarchyManager();
}

core::IVisualizerManager* python::PythonPluginUtils::getVisualizerManager()
{
	return plugin::getVisualizerManager();
}

core::IDataManagerReader* python::PythonPluginUtils::getDataManagerReader()
{
	return plugin::getDataManagerReader();
}

void python::PythonPluginUtils::pluginLog(const std::string& s)
{
	PLUGIN_LOG_DEBUG(s);
}

