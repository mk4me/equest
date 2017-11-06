/********************************************************************
    created:  2011/04/21
    created:  21:4:2011   7:59
    filename: PluginCommon.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__PLUGINCOMMON_H__
#define HEADER_GUARD_CORE__PLUGINCOMMON_H__

#include <utils/Filesystem.h>
#include <corelib/IApplication.h>
#include <corelib/IPath.h>
#include <loglib/ILog.h>
#include <utils/Macros.h>
#include <corelib/IDataManager.h>
#include <corelib/IPlugin.h>
#include <corelib/IRegisteredDataTypesManagerReader.h>

namespace plugin 
{
    //! Zmienna defininowana przez makro tworzące pluginy. Tylko do użytku wewnętrznego.
    extern core::IApplication * __coreApplication;

    //! Makro definiujące zmienną przechowującą managery. Automatycznie używane w pluginach.
    #define PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR namespace plugin { core::IApplication * __coreApplication = nullptr; }

	//! \return Kontekst aplikacji
	inline core::IApplication* getApplication()
	{
		return __coreApplication;
	}

    //! \return Bieżąca instancja data managera readera. Rozwiązanie w ten sposób, w stosunku do
    //! klasycznego upublicznienia tylko nagłówków funkcji i schowania definicji, pozwala
    //! na rozwijanie, więc jest potencjalnie szybsze.
    inline core::IDataManagerReader* getDataManagerReader()
    {
		return getApplication()->dataManagerReader();
    }

	inline core::IServiceManager* getServiceManager()
	{
		return getApplication()->serviceManager();
	}
	 
	 inline core::IFileManagerReader * getFileDataManagerReader()
	 {
		 return getApplication()->fileDataManagerReader();
	 }

	 inline core::IStreamManagerReader * getStreamDataManagerReader()
	 {
		 return getApplication()->streamDataManagerReader();
	 }

	inline core::IRegisteredDataTypesManagerReader* getRegisteredDataTypesManagerReader()
	{
		return getApplication()->registeredDataTypesManagerReader();
	}

	inline core::IDataHierarchyManagerReader* getDataHierarchyManagerReader()
	{
		return getApplication()->dataHierarchyManagerReader();
	}

	inline core::ISourceManager* getSourceManager()
	{
		return getApplication()->sourceManager();
	}

	inline core::IParserManagerReader* getParserManagerReader()
	{
		return getApplication()->parserManagerReader();
	}

    //! \return Interfejs dostępu do ścieżek aplikacji
	inline core::IPath* getPaths()
	{
		return getApplication()->paths();
	}

	//! \return Interfejs dostępu do ścieżek aplikacji
	inline core::IVisualizerManager* getVisualizerManager()
	{
		return getApplication()->visualizerManager();
	}

    //! \return Interfejs logowania informacji
	inline loglib::ILog* getLogInterface()
	{
		return getApplication()->log();
	}

	//! \return Interfejs logowania informacji
	inline core::IThreadPool* getThreadPool()
	{
		return getApplication()->threadPool();
	}

	//! \return Interfejs logowania informacji
	inline core::IJobManager* getJobManager()
	{
		return getApplication()->jobManager();
	}

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const utils::Filesystem::Path& getResourcesPath()
    {
        return getPaths()->getResourcesPath();
    }

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const utils::Filesystem::Path& getApplicationDataPath()
    {
        return getPaths()->getApplicationDataPath();
    }

	//! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const utils::Filesystem::Path& getUserApplicationDataPath()
    {
        return getPaths()->getUserApplicationDataPath();
    }

    //! Pomocnica metoda upraszczająca odwołanie do katalogów.
    inline const utils::Filesystem::Path& getUserDataPath()
    {
        return getPaths()->getUserDataPath();
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline utils::Filesystem::Path getResourcePath(const utils::Filesystem::Path& path)
    {
        return getResourcesPath()/path;
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline utils::Filesystem::Path getApplicationDataPath(const utils::Filesystem::Path& path)
    {
        return getApplicationDataPath()/path;
    }

	    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline utils::Filesystem::Path getUserApplicationDataPath(const utils::Filesystem::Path& path)
    {
        return getUserApplicationDataPath()/path;
    }

    //! \return Pomocnicza metoda do pobierania ścieżek.
    inline utils::Filesystem::Path getUserDataPath(const utils::Filesystem::Path& path)
    {
        return getUserDataPath()/path;
    }

	//! \return Pomocnicza metoda do pobierania ścieżek.
	inline const utils::Filesystem::Path & getPluginPath()
	{
		return getPaths()->getPluginPath();
	}

	//! \return Pomocnicza metoda do pobierania ścieżek.
	inline utils::Filesystem::Path getPluginPath(const utils::Filesystem::Path & path)
	{
		return getPluginPath()/path;
	}

} // namespace core

//! Makro logujące informację testową
#define PLUGIN_LOG_DEBUG(msg)	LOG_DEBUG(plugin::getLogInterface(), msg)
//! Makro logujące błąd
#define PLUGIN_LOG_ERROR(msg)	LOG_ERROR(plugin::getLogInterface(), msg)
//! Makro logujące informację
#define PLUGIN_LOG_INFO(msg)	LOG_INFO(plugin::getLogInterface(), msg)
//! Makro logujące ostrzeżenia
#define PLUGIN_LOG_WARNING(msg)	LOG_WARNING(plugin::getLogInterface(), msg)

//! Makro logujące informację testową
#define PLUGIN_LOG_NAMED_DEBUG(name, msg)	SUB_LOG_DEBUG(plugin::getLogInterface(), name, msg)
//! Makro logujące błąd
#define PLUGIN_LOG_NAMED_ERROR(name, msg)	SUB_LOG_ERROR(plugin::getLogInterface(), name, msg)
//! Makro logujące informację
#define PLUGIN_LOG_NAMED_INFO(name, msg)	SUB_LOG_INFO(plugin::getLogInterface(), name, msg)
//! Makro logujące ostrzeżenia
#define PLUGIN_LOG_NAMED_WARNING(name, msg)	SUB_LOG_WARNING(plugin::getLogInterface(), name, msg)

#endif  // HEADER_GUARD_CORE__PLUGINCOMMON_H__
