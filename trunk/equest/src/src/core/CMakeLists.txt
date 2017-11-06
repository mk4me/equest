BEGIN_PROJECT("static")

if(CONFIG_CORE_ENABLE_LOG4CXX)
	MARK_AS_PREREQUISITES("LOG4CXX")
endif()

CONFIG_OPTION(CORE_ENABLE_LEAK_DETECTION "Enable leak detection?" OFF)

SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)

CONFIGURE_PRIVATE_HEADER("Config.h.in" "Config.h")

SEARCH_DEPENDENCY_CONFIGURATION_DEFINES("Config.h" "CORE_ENABLE_LEAK_DETECTION;CORE_ENABLE_LOG4CXX;CORE_ENABLE_LOGGING")

SET_PRIVATE_HEADERS(
	ApplicationDescription.h
	LanguagesManager.h
	LanguagesLoader.h
	LanguagesHelper.h
	LanguageChangeFilter.h
	Path.h
	Application.h
	ParserManager.h
	ApplicationCommon.h
	DataHierarchyManager.h
	RegisteredDataTypesManager.h
	DataManager.h
	FileDataManager.h
	StreamDataManager.h
	PluginApplication.h
	PluginPath.h
	SourceManager.h
	LogInitializer.h
	ServiceManager.h
	VisualizerManager.h
	Log.h
	PluginLoader.h
	CorePCH.h
	Plugin.h
	#ConverterManager.h
	#Converter.h
	JobManager.h
	ThreadPool.h
)

SET_PUBLIC_HEADERS (
	AppInitializer.h
)

SET_SOURCE_FILES(
	ApplicationDescription.cpp
	#ConverterManager.cpp
	#Converter.cpp
	LanguageChangeFilter.cpp
	LanguagesManager.cpp
	LanguagesLoader.cpp
	LanguagesHelper.cpp
	AppInitializer.cpp
	Path.cpp
	Application.cpp
	ParserManager.cpp
	Log.cpp
	DataHierarchyManager.cpp
	RegisteredDataTypesManager.cpp
	DataManager.cpp
	FileDataManager.cpp
	StreamDataManager.cpp
	PluginApplication.cpp
	PluginPath.cpp
	SourceManager.cpp
	LogInitializer.cpp
	PluginLoader.cpp
	ServiceManager.cpp
	VisualizerManager.cpp
	CorePCH.cpp
	Plugin.cpp
	JobManager.cpp
	ThreadPool.cpp
)

SET_MOC_FILES(
	Application.h
	LanguageChangeFilter.h
)

SET_RC_FILES (
	coreIcons.qrc
)

SET_PRECOMPILED_HEADER (CorePCH.h CorePCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

MARK_DEPLOY_RESOURCES_AS_MODIFIABLE(
	"settings/log.ini"
)

END_PROJECT()