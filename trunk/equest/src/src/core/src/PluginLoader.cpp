#include "CorePCH.h"

#include <utils/Filesystem.h>
#include <corelib/Version.h>
#include "PluginLoader.h"
#include <corelib/IPlugin.h>
#include <corelib/PluginCommon.h>
#include "PluginApplication.h"
#include "ApplicationCommon.h"

#include <regex>

#if defined(_WINDOWS)
#define NOMINMAX
#include <windows.h>
#elif defined(__UNIX__)
#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#endif

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

PluginLoader::PluginLoader(const utils::Filesystem::Path & pluginsPath)
{
    paths.push_back(pluginsPath);
}

PluginLoader::~PluginLoader()
{
    unloadPlugins();
}

std::string PluginLoader::pluginIdentyfier(const std::string & name,
	const utils::Filesystem::Path & path, const core::UniqueID & id)
{
	std::stringstream ss;
	ss << name << " (ID: " << id << ") from " << path.string();
	return ss.str();
}

void PluginLoader::deinitialize()
{
	for (auto & p : plugins)
	{
		try{
			CORE_LOG_DEBUG("Deinitializing plugin " << p.identyfier);
			p.plugin->deinitialize();
			CORE_LOG_DEBUG("Plugin deinitialized successfuly");
		}
		catch (std::exception & e){
			CORE_LOG_DEBUG("Failed to deinitialized plugin: " << e.what());
		}
		catch (...){
			CORE_LOG_DEBUG("Failed to deinitialized plugin with UNKNOWN error");
		}
	}
}

void PluginLoader::reset()
{
	for (auto & p : plugins)
	{
		try{
			CORE_LOG_DEBUG("Reseting plugin " << p.identyfier);
			p.plugin.reset();
			CORE_LOG_DEBUG("Plugin reset successfuly");

		}
		catch (std::exception & e){
			CORE_LOG_DEBUG("Failed to reset plugin: " << e.what());
		}
		catch (...){
			CORE_LOG_DEBUG("Failed to reset plugin with UNKNOWN error");
		}
	}
}

void PluginLoader::clear()
{
    // wyczyszczenie ścieżek
    Paths().swap(paths);

	for(auto & p : plugins)
	{	
		unloadSharedLibrary(p.handle);		
	}

	Plugins().swap(plugins);
}

void PluginLoader::unloadPlugins()
{
    try{
        clear();
    }
    catch(std::exception& e){
        CORE_LOG_ERROR("PluginLoader: Error unloading plugins " << e.what());
    }
    catch(...){
        CORE_LOG_ERROR("PluginLoader: UNKNOWN error unloading plugins");
    }
}

void PluginLoader::load()
{

#if defined(_WINDOWS)
	static const std::regex pluginFilter("^plugin_.*\\.dll$");
#elif defined(__UNIX__)
    // GCC cos nie radzi sobie ze znakami początku i końca linii !?
	static const std::regex pluginFilter("libplugin_.*\\.so");
#endif

	for(auto pathIT = paths.begin(); pathIT != paths.end(); ++pathIT) {

		auto localFiles = utils::Filesystem::listFiles(*pathIT, true);

		for (auto fileIT = localFiles.begin(); fileIT != localFiles.end(); ++fileIT){

			// Skip if no match
			if (!std::regex_match((*fileIT).leaf().string(), pluginFilter)) continue;

			addPlugIn(*fileIT);
		}

	}

	/*std::list<core::Job<void>> jobsList;

	auto jm = core::getJobManager();

	for(auto pathIT = paths.begin(); pathIT != paths.end(); ++pathIT) {

		jobsList.push_back(jm->create("core", "Plugin loader", [this, jm](utils::Filesystem::Path path){

			auto localFiles = utils::Filesystem::listFiles(path, true);
			std::list<core::Job<bool>> jobsList;

			for (auto fileIT = localFiles.begin(); fileIT != localFiles.end(); ++fileIT){

				// Skip if no match
				if (!std::regex_match((*fileIT).leaf().string(), pluginFilter)) continue;
				
				jobsList.push_back(jm->create("core", "Plugin loader", &PluginLoader::addPlugIn, this, *fileIT));
			}

			for (auto & job : jobsList){
				job.start();
			}

			for (auto & job : jobsList){
				job.wait();
			}
		}, *pathIT));
	}

	for (auto & job : jobsList){
		job.start();
	}

	for (auto & job : jobsList){
		job.wait();
	}*/
}

HMODULE PluginLoader::loadSharedLibrary(const utils::Filesystem::Path & path)
{
#if defined(_WINDOWS)
	return ::LoadLibrary( path.string().c_str() );
#elif defined(__UNIX__)
	return dlopen(path.string().c_str(), RTLD_LAZY);
#else
#error "Unsupported system for loading shared libraries"
	return 0;
#endif
}

void PluginLoader::unloadSharedLibrary(HMODULE library)
{
	if(library){
#if defined(_WINDOWS)
		FreeLibrary(library);
#elif defined(__UNIX__)
		dlclose(library);
#else
	#error "Unsupported system for unloading shared libraries"
#endif
	}
}

const std::string PluginLoader::lastLoadSharedLibraryError()
{
#if defined(_WINDOWS)
	DWORD err = ::GetLastError();
	LPSTR errStr;
	if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, err, 0, reinterpret_cast<LPSTR>(&errStr), 0, nullptr)) {
		// usuwanie CRLF
		LPSTR p = strchr(errStr, '\r');
		if ( p ) {
			*p = '\0';
		}

		std::string ret(errStr);

		::LocalFree(errStr);

		return ret + " Error code: " + boost::lexical_cast<std::string>(err);
	}

	return std::string("Unknown error");
#elif defined(__UNIX__)
	return std::string(dlerror());
#else
#error "Unsupported system for loading shared libraries"
	return std::string("Unknown error");
#endif
}

bool PluginLoader::addPlugIn( const utils::Filesystem::Path& path )
{
    HMODULE library = loadSharedLibrary(path.string());
    if ( library != 0) {
        try{
            if (checkPluginVersion(library, path) && checkLibrariesVersions(library, path) && checkPluginBuildType(library, path) ) {
                PluginPtr plugin(new Plugin());
				plugin->setPath(path);
				auto procSetDescription = loadProcedure<Plugin::SetPluginDescriptionFunction>(library, STRINGIZE(CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME));
				if( procSetDescription != 0 ){
					procSetDescription(plugin.get());
				}else{
					CORE_LOG_DEBUG(path << " is a plugin, but finding " << STRINGIZE(CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME) << " failed.");
					return false;
				}

				auto procInitializePluginContext = loadProcedure<Plugin::InitializePluginContextFunction>(library, STRINGIZE(CORE_INITIALIZE_PLUGIN_CONTEXT_FUNCTION_NAME));
				if (procInitializePluginContext != 0) {
					return onAddPlugin(plugin, library, procInitializePluginContext);
                } else {
					CORE_LOG_DEBUG(path << " is a plugin, but finding " << STRINGIZE(CORE_INITIALIZE_PLUGIN_CONTEXT_FUNCTION_NAME) << " failed.");
                }
            }
        }catch(std::exception & e){
            CORE_LOG_DEBUG(path << " is a plugin, but trying to check version or libraries failed with error: " << e.what());
        }catch(...){
            CORE_LOG_DEBUG(path << " is a plugin, but trying to check version or libraries failed with UNKNOWN error");
        }
    } else  {
        CORE_LOG_ERROR("Error \"" << lastLoadSharedLibraryError() << "\" during loading " << path << ".");
    }

	unloadSharedLibrary(library);
    return false;
}

bool PluginLoader::onAddPlugin(PluginPtr plugin, HMODULE library, Plugin::InitializePluginContextFunction initializeContextFunction)
{
	PluginData pData;
    CORE_LOG_INFO("Loading plugin " << plugin->getPath());

	auto pluginName = plugin->name();
	if(pluginName.empty() == true){
		plugin->setName(plugin->getPath().stem().string());
		CORE_LOG_WARNING("Plugin name for plugin loaded from " << plugin->getPath() << " was empty. Setting dynamic library file name as plugin name: " << plugin->name() );
	}

	pData.coreApplication.reset(new PluginApplication(plugin->name()));

    // próba załadowania zawartosci pluginu i zainicjowania kontekstu aplikacji
    try {
		
		initializeContextFunction(plugin.get(), pData.coreApplication.get());
        
    } catch ( std::exception& ex ) {
        CORE_LOG_ERROR("Error loading plugin content and initializing application context for " << plugin->getPath() << ": " << ex.what());
        return false;
    } catch ( ... ) {
        CORE_LOG_ERROR("Unknown error loading plugin content and initializing application context for " << plugin->getPath());
        return false;
    }

	auto procLoadLogicContent = loadProcedure<Plugin::LoadLogicContentFunctionFunction>(library, STRINGIZE(CORE_PLUGIN_LOAD_LOGIC_CONTENT_FUNCTION_NAME));
	if (procLoadLogicContent != 0) {
		procLoadLogicContent(plugin.get());
	}
	else {
		CORE_LOG_DEBUG(plugin->getPath() << " is a plugin, but finding " << STRINGIZE(CORE_PLUGIN_LOAD_LOGIC_CONTENT_FUNCTION_NAME) << " failed.");
		return false;
	}

	if(plugin->empty() == true){
		CORE_LOG_INFO("Skipping loading of an empty plugin " << plugin->getPath());
		return false;
	}

	pData.plugin = plugin;		
	pData.handle = library;

    bool pluginIDFound = false;
    core::PluginPtr collidingPlugin;

	std::lock_guard<std::mutex> lock(sync);

    //szukamy pluginu o podanym ID - jeśli nie ma ladujemy, w przeciwnym wypadku info i nie dodajemy
    for(auto it = plugins.begin(); it != plugins.end(); ++it){
        if( (*it).plugin->ID() == pData.plugin->ID()){
            pluginIDFound = true;
            collidingPlugin = (*it).plugin;
            break;
        }
    }

    if(pluginIDFound == false){	

        plugins.push_back( pData );
		pData.identyfier = pluginIdentyfier(pData.plugin->name(), pData.plugin->getPath(), pData.plugin->ID());
        CORE_LOG_INFO("Successfully loaded plugin " << pData.identyfier);

    }else{
        CORE_LOG_WARNING("Plugin with given ID " << pData.plugin->ID() << " already exist. Plugin " << plugin->getPath() << " NOT loaded to application! Collision with plugin loaded from: " << collidingPlugin->getPath() );
    }

    return !pluginIDFound;
}

bool PluginLoader::checkLibrariesVersions( HMODULE library, const utils::Filesystem::Path& path )
{
    // pobranie wersji bibliotek
    auto libsVerProc = loadProcedure<Plugin::GetLibrariesVersionFunction>(library, STRINGIZE(CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME));
    if ( libsVerProc ) {
        int boostVer, qtVer, stlVer;
        libsVerProc(&boostVer, &qtVer, &stlVer);
        CORE_LOG_DEBUG(path << " boost: " << boostVer << "; Qt: " << std::hex << qtVer << "; STL: " << std::dec << stlVer);
        bool success = true;
        // boost/version.hpp - sprawdzenie major i minor
        if ( boostVer / 100 != BOOST_VERSION / 100 ) {
            CORE_LOG_ERROR(path << " has incompatible boost version: " << boostVer);
            success = false;
        }
        // QtCore/QGlobal.h - sprawdzenie tylko wersji Major
        if ( (qtVer >> 16) != (QT_VERSION >> 16) ) {
            CORE_LOG_ERROR(path << " has incompatible Qt version: " << qtVer);
            success = false;
        }
        //
        if ( stlVer != CORE_CPPLIB_VER ) {
            if ( stlVer == -1 ) {
                CORE_LOG_ERROR(path << " has incompatible STL version: " << "unknown");
            } else {
                CORE_LOG_ERROR(path << " has incompatible STL version: " << stlVer);
            }
            success = false;
        }
        return success;
    } else {
        CORE_LOG_ERROR(path << " is a plugin, but finding " << STRINGIZE(CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME) << " failed");
        return false;
    }
}

bool PluginLoader::checkPluginVersion( HMODULE library, const utils::Filesystem::Path& path )
{
	auto versionProc = loadProcedure<Plugin::GetAPIVersionFunction>(library, STRINGIZE(CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME));
	if ( versionProc ) {
		int pMajor;
		int pMinor;
		int pPatch;
		versionProc(&pMajor, &pMinor, &pPatch);
		if ( pMajor != CORE_API_MAJOR || pMinor > CORE_API_MINOR) {
			CORE_LOG_ERROR(path << " has obsolete interface version - should be "
				<< Version::formatedVersion() << ", is "
				<< Version::formatedVersion(Version(pMajor, pMinor, pPatch)));
			return false;
		} else {
			return true;
		}
	} else {
		CORE_LOG_ERROR(path<<" is a shared library, but finding "<<STRINGIZE(CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME)<<" failed. Is it a plugin or library?");
		return false;
	}
}

bool PluginLoader::checkPluginBuildType( HMODULE library, const utils::Filesystem::Path& path )
{
    auto buildTypeProc = loadProcedure<Plugin::GetBuildTypeFunction>(library, STRINGIZE(CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME));
    if ( buildTypeProc ) {
        int buildType = buildTypeProc();
        if ( buildType != CORE_PLUGIN_BUILD_TYPE ) {
            CORE_LOG_ERROR(path<<" has obsolete interface buildType; should be "<<CORE_PLUGIN_BUILD_TYPE<<", is "<<buildType);
            return false;
        } else {
            return true;
        }
    } else {
        CORE_LOG_ERROR(path<<" is a shared library, but finding "<<STRINGIZE(CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME)<<" failed. Is it a plugin or library?");
        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
