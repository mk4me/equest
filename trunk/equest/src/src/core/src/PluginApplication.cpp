#include "CorePCH.h"
#include "PluginApplication.h"
#include "ApplicationCommon.h"
#include "PluginPath.h"
#include "ParserManager.h"
#include "RegisteredDataTypesManager.h"
#include "LanguagesManager.h"
#include "ApplicationDescription.h"
#include "DataHierarchyManager.h"

using namespace core;

PluginApplication::PluginApplication(const std::string & pluginName)
	: path_(new PluginPath(pluginName)),
	log_(getPrototypeLogInterface()->subLog("plugin." + pluginName))
{

}

PluginApplication::~PluginApplication()
{

}

IPath * PluginApplication::paths()
{
	return path_.get();
}

IDataManagerReader * PluginApplication::dataManagerReader()
{
	return getDataManager();
}

IPluginStorage * PluginApplication::pluginStorage()
{
	return nullptr;
}

loglib::ILog * PluginApplication::log()
{
	return log_.get();
}

ISourceManager * PluginApplication::sourceManager()
{
	return getSourceManager();
}

IServiceManager * PluginApplication::serviceManager()
{
	return getServiceManager();
}

IParserManagerReader * PluginApplication::parserManagerReader()
{
	return getParserManager();
}

IStreamManagerReader * PluginApplication::streamDataManagerReader()
{
	return getStreamDataManager();
}

IFileManagerReader * PluginApplication::fileDataManagerReader()
{
	return getFileDataManager();
}

IDataHierarchyManagerReader * PluginApplication::dataHierarchyManagerReader()
{
	return getDataHierarchyManager();
}

IRegisteredDataTypesManagerReader * PluginApplication::registeredDataTypesManagerReader()
{
	return getRegisteredDataTypesManager();
}

IVisualizerManager * PluginApplication::visualizerManager()
{
	return getVisualizerManager();
}

IThreadPool * PluginApplication::threadPool()
{
	return getThreadPool();
}

IJobManager * PluginApplication::jobManager()
{
	return getJobManager();
}

IApplicationDescription * PluginApplication::description()
{
	return getDescription();
}

ILanguagesManager * PluginApplication::languagesManager()
{
	return getLanguageManager();
}

MainViewApplication::MainViewApplication()
{

}

MainViewApplication::~MainViewApplication()
{

}

IApplicationDescription * MainViewApplication::description()
{
	return getDescription();
}

ILanguagesManager * MainViewApplication::languagesManager()
{
	return getLanguageManager();
}

IPath * MainViewApplication::paths()
{
	return getPaths();
}

IDataManagerReader * MainViewApplication::dataManagerReader()
{
	return getDataManager();
}

IPluginStorage * MainViewApplication::pluginStorage()
{
	return nullptr;
}

loglib::ILog * MainViewApplication::log()
{
	return getLogInterface();
}

ISourceManager * MainViewApplication::sourceManager()
{
	return getSourceManager();
}

IServiceManager * MainViewApplication::serviceManager()
{
	return getServiceManager();
}

IParserManagerReader * MainViewApplication::parserManagerReader()
{
	return getParserManager();
}

IStreamManagerReader * MainViewApplication::streamDataManagerReader()
{
	return getStreamDataManager();
}

IFileManagerReader * MainViewApplication::fileDataManagerReader()
{
	return getFileDataManager();
}

IDataHierarchyManagerReader * MainViewApplication::dataHierarchyManagerReader()
{
	return getDataHierarchyManager();
}

IRegisteredDataTypesManagerReader * MainViewApplication::registeredDataTypesManagerReader()
{
	return getRegisteredDataTypesManager();
}

IVisualizerManager * MainViewApplication::visualizerManager()
{
	return getVisualizerManager();
}

ThreadPool * MainViewApplication::threadPool()
{
	return getThreadPool();
}

JobManager * MainViewApplication::jobManager()
{
	return getJobManager();
}