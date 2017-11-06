/********************************************************************
    created:  2013/01/10
    created:  10:1:2013   8:54
    filename: ApplicationCommon.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___APPLICATIONCOMMON_H__
#define HEADER_GUARD___APPLICATIONCOMMON_H__

#include <string>
#include "Log.h"
#include "Path.h"
#include <utils/Filesystem.h>
#include "Application.h"
#include <utils/Macros.h>
#include <boost/uuid/uuid.hpp>

namespace core {

//! Zmienna defininowana przez makro tworz¹ce pluginy. Tylko do u¿ytku wewnêtrznego.
extern Application * __application;

//! Makro definiuj¹ce zmienn¹ przechowuj¹c¹ managery. Automatycznie u¿ywane w pluginach.
#define DEFINE_CORE_APPLICATION_ACCESSOR namespace core { Application * __application = nullptr; }

inline Application * getApplication()
{
	return __application;
}

//! \return Bie¿¹ca instancja data managera readera. Rozwi¹zanie w ten sposób, w stosunku do
//! klasycznego upublicznienia tylko nag³ówków funkcji i schowania definicji, pozwala
//! na rozwijanie, wiêc jest potencjalnie szybsze.
inline DataManager* getDataManager()
{
	return __application->dataManager();
}

inline StreamDataManager* getStreamDataManager()
{
	return __application->streamDataManager();
}

inline FileDataManager* getFileDataManager()
{
	return __application->fileDataManager();
}

inline DataHierarchyManager* getDataHierarchyManager()
{
	return __application->dataHierarchyManager();
}

inline RegisteredDataTypesManager* getRegisteredDataTypesManager()
{
	return __application->registeredDataTypesManager();
}

inline ParserManager* getParserManager()
{
	return __application->parserManager();
}

inline ServiceManager* getServiceManager()
{
	return __application->serviceManager();
}

inline SourceManager* getSourceManager()
{
	return __application->sourceManager();
}

inline VisualizerManager* getVisualizerManager()
{
	return __application->visualizerManager();
}

inline ThreadPool* getThreadPool()
{
	return __application->threadPool();
}

inline JobManager* getJobManager()
{
	return __application->jobManager();
}

//! \return Interfejs dostêpu do œcie¿ek aplikacji
inline LanguagesManager* getLanguageManager()
{
	return __application->languageManager();
}

//! \return Interfejs dostêpu do œcie¿ek aplikacji
inline ApplicationDescription * getDescription()
{
	return __application->description();
}

//! \return Interfejs dostêpu do œcie¿ek aplikacji
inline Path* getPaths()
{
	return __application->paths();
}

//! \return Interfejs logowania informacji
inline loglib::ILog* getLogInterface()
{
	return __application->logger();
}

//! \return Interfejs logowania informacji
inline loglib::ILog* getExceptionLogInterface()
{
	return __application->logger();
}

//! \return Interfejs logowania informacji
inline loglib::ILog* getPrototypeLogInterface()
{
	return __application->loggerPrototype();
}

//! Pomocnica metoda upraszczaj¹ca odwo³anie do katalogów.
inline const utils::Filesystem::Path& getResourcesPath()
{
	return getPaths()->getResourcesPath();
}

//! Pomocnica metoda upraszczaj¹ca odwo³anie do katalogów.
inline const utils::Filesystem::Path& getApplicationDataPath()
{
	return getPaths()->getApplicationDataPath();
}

//! Pomocnica metoda upraszczaj¹ca odwo³anie do katalogów.
inline const utils::Filesystem::Path& getUserDataPath()
{
	return getPaths()->getUserDataPath();
}

//! \return Pomocnicza metoda do pobierania œcie¿ek.
inline utils::Filesystem::Path getResourcePath(const utils::Filesystem::Path& path)
{
	return getResourcesPath()/path;
}

//! \return Pomocnicza metoda do pobierania œcie¿ek.
inline utils::Filesystem::Path getApplicationDataPath(const utils::Filesystem::Path& path)
{
	return getApplicationDataPath()/path;
}

//! \return Pomocnicza metoda do pobierania œcie¿ek.
inline utils::Filesystem::Path getUserDataPath(const utils::Filesystem::Path& path)
{
	return getUserDataPath()/path;
}

//! \return Pomocnicza metoda do pobierania œcie¿ek.
inline const utils::Filesystem::Path & getPluginPath()
{
	return getPaths()->getPluginPath();
}

//! \return Pomocnicza metoda do pobierania œcie¿ek.
inline utils::Filesystem::Path getPluginPath(const utils::Filesystem::Path & path)
{
	return getPluginPath()/path;
}

}


//! Makro loguj¹ce informacjê testow¹
#define CORE_LOG_DEBUG(msg)		LOG_DEBUG(core::getLogInterface(), msg)
//! Makro loguj¹ce b³¹d
#define CORE_LOG_ERROR(msg)		LOG_ERROR(core::getLogInterface(), msg)
//! Makro loguj¹ce informacjê
#define CORE_LOG_INFO(msg)		LOG_INFO(core::getLogInterface(), msg)
//! Makro loguj¹ce ostrze¿enia
#define CORE_LOG_WARNING(msg)	LOG_WARNING(core::getLogInterface(), msg)

//! Makro loguj¹ce informacjê testow¹
#define CORE_LOG_NAMED_DEBUG(name, msg)		SUB_LOG_DEBUG(core::getLogInterface(), name, msg)
//! Makro loguj¹ce b³¹d
#define CORE_LOG_NAMED_ERROR(name, msg)		SUB_LOG_ERROR(core::getLogInterface(), name, msg)
//! Makro loguj¹ce informacjê
#define CORE_LOG_NAMED_INFO(name, msg)		SUB_LOG_INFO(core::getLogInterface(), name, msg)
//! Makro loguj¹ce ostrze¿enia
#define CORE_LOG_NAMED_WARNING(name, msg)	SUB_LOG_WARNING(core::getLogInterface(), name, msg)

//! Makro loguj¹ce informacjê testow¹
#define LOG_NAMED_DEBUG(name, msg)		SUB_LOG_DEBUG(core::getPrototypeLogInterface(), name, msg)
//! Makro loguj¹ce b³¹d
#define LOG_NAMED_ERROR(name, msg)		SUB_LOG_ERROR(core::getPrototypeLogInterface(), name, msg)
//! Makro loguj¹ce informacjê
#define LOG_NAMED_INFO(name, msg)		SUB_LOG_INFO(core::getPrototypeLogInterface(), name, msg)
//! Makro loguj¹ce ostrze¿enia
#define LOG_NAMED_WARNING(name, msg)	SUB_LOG_WARNING(core::getPrototypeLogInterface(), name, msg)

#endif	//	HEADER_GUARD___APPLICATIONCOMMON_H__
