/********************************************************************
	created:  2010/10/04
	created:  4:10:2010   15:30
	filename: Plugin.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_PLUGIN_H__
#define __HEADER_GUARD__CORE_PLUGIN_H__

#include <vector>
#include <string>

#include <corelib/Version.h>

#include <boost/version.hpp>
#include <QtCore/QtGlobal>
#include <utils/Macros.h>

#include <corelib/PluginCommon.h>
#include <loglib/ILog.h>
#include <corelib/IPath.h>
#include <utils/SmartPtr.h>
#include <corelib/IService.h> 
#include <corelib/ISource.h>
#include <corelib/IParser.h>
#include <corelib/IVisualizer.h>
#include <utils/Export.h>
#include <corelib/IPluginDescription.h>

//! Weryfikacja typu bilda pluginu
#ifdef _DEBUG
    #define CORE_PLUGIN_BUILD_TYPE 0
#else
    #define CORE_PLUGIN_BUILD_TYPE 1
#endif

#ifdef _CPPLIB_VER
#define CORE_CPPLIB_VER _CPPLIB_VER
#else
#define CORE_CPPLIB_VER -1
#endif

//! Nazwa funkcji pobierającej numer wersji biblioteki core z którą był zbudowany plugin
#define CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME CoreGetPluginAPIVersion
//! Nazwa funkcji pobierającej typ builda pluginu - debug/release
#define CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME CoreGetPluginBuildType
//! Nazwa funkcji inicjującej kontekst pluginu - opis, język, dostęp do zasobów aplikacji
#define CORE_INITIALIZE_PLUGIN_CONTEXT_FUNCTION_NAME CoreInitializePluginContext
//! Nazwa funkcji ładującej elementy logiki pluginu
#define CORE_PLUGIN_LOAD_LOGIC_CONTENT_FUNCTION_NAME CoreLoadLogicContent
//! Nazwa funkcji pobierającej wersję bibliotek od których zależny jest również core
#define CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME CoreGetLibrariesVersions
//! Nazwa fucnkji inicjującej opis pluginu
#define CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME CoreInitializePluginDescription

//! Ustawia dodatkowy opis pluginu
#define CORE_PLUGIN_SET_DESCRIPTION(shortName, description)	\
{															\
	plugin->setDescription(shortName, description);			\
}

//! Rozpoczyna rejestrację pluginu
//! \param name Nazwa pluginu
//! \param id ID pluginu
//! \param id langCode Kod domyślnego języka pluginy wg ISO639
//! \param init Funkcja inicjująca plugin
//! \param deinit Funkcja deinicjująca plugin - odłancza go od wszystkich zewnętrznych zasobów
//! \param vendorName Nazwa dostawcy plugninu
//! \param vendorShortName Krótka nazwa dostawcy pluginu
//! \param vendorDescription Opis dostawcy pluginu
//! \param vendorContact Kontakt do dostawcy pluginu
//! \param versionMajor Nr główny wersji pluginu
//! \param versionMinor Nr dodatkowy wersji pluginu
//! \param versionPatch Nr wersji patcha wersji pluginu
#define CORE_EXT_PLUGIN_BEGIN(name, id, langCode, \
	init, lateInit, deinit, \
	vendorName, vendorShortName, vendorDescription, vendorContact, \
	versionMajor, versionMinor, versionPatch) \
PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR                                 \
extern "C" UTILS_DECL_EXPORT void CORE_GET_PLUGIN_API_VERSION_FUNCTION_NAME( \
	int* major, int* minor, int* patch)									\
{                                                                       \
	*major = CORE_API_MAJOR;				                            \
	*minor = CORE_API_MINOR;											\
	*patch = CORE_API_PATCH;											\
}                                                                       \
extern "C" UTILS_DECL_EXPORT unsigned CORE_GET_PLUGIN_BUILD_TYPE_FUNCTION_NAME() \
{                                                                       \
	return CORE_PLUGIN_BUILD_TYPE;                                      \
}                                                                       \
extern "C" UTILS_DECL_EXPORT void CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME(  \
	int* boostVersion, int* qtVersion, int* stlVersion)                 \
{                                                                       \
	*boostVersion = BOOST_VERSION;                                      \
	*qtVersion = QT_VERSION;                                            \
	*stlVersion = CORE_CPPLIB_VER;                                      \
}                                                                       \
extern "C" UTILS_DECL_EXPORT void CORE_INITIALIZE_PLUGIN_DESCRIPTION_FUNCTION_NAME(core::IPlugin * plugin) \
{																		\
	plugin->setName(name);												\
	plugin->setID(id);													\
	plugin->setDefaultLanguageCode(langCode);							\
	plugin->setVendor(vendorName, vendorShortName, vendorDescription, vendorContact); \
	plugin->setVersion(versionMajor, versionMinor, versionPatch);		\
}																		\
extern "C" UTILS_DECL_EXPORT void CORE_INITIALIZE_PLUGIN_CONTEXT_FUNCTION_NAME(core::IPlugin * plugin, \
	core::IApplication* coreApplication)								\
{                                                                       \
	plugin::__coreApplication = coreApplication;						\
	plugin->setLoadDescription(init, lateInit, deinit);						\
}																		\
extern "C" UTILS_DECL_EXPORT void CORE_PLUGIN_LOAD_LOGIC_CONTENT_FUNCTION_NAME(core::IPlugin * plugin) \
{

//! Rozpoczyna rejestrację pluginu
//! \param name Nazwa pluginu
//! \param id ID pluginu
#define CORE_PLUGIN_BEGIN(name, id)	CORE_EXT_PLUGIN_BEGIN(name, id, "en", \
	nullptr, nullptr, nullptr, \
	"Polsko-Japońska Akademia Technik Komputerowych, Oddział zamiejscowy w Bytomiu", "PJATK Bytom", "Uczelnia prywatna", "marek.kulbacki@gmail.com", \
	1, 0, 0)

//! Kończy rejestrację pluginu.
#define CORE_PLUGIN_END                                                 \
}

//! Dodaje usługę zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SERVICE(className)                              \
{																		\
	try { plugin->addService( plugin::IServicePtr(new className) ); }	\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading service class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading service class" << #className); } \
}

//! Dodaje źródło danych DM zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SOURCE(className)                              \
{																		\
	try { plugin->addSource( plugin::ISourcePtr(new className) ); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading source class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading source class " << #className); } \
}    

//! Dodaje parser zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_PARSER(className)                               \
{																		\
	try { plugin->addParserPrototype( plugin::IParserPtr(new className) ); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading parser class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading parser class " << #className); } \
}     

//! Dodaje wizualizator zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_VISUALIZER(className)                           \
{																		\
	try { plugin->addVisualizerPrototype( plugin::IVisualizerPtr(new className) ); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading visualizer class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading visualizer class " << #className); } \
} 

/*
//! Dodaje wizualizator zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_VISUALIZER_ACTION(visualizerID, actionClassName)                           \
{																		\
	try { plugin->addVisualizerPrototype( plugin::IVisualizerPtr(new className) ); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading visualizer class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading visualizer class " << #className); } \
}
*/

//! Dodanie nowego typu domenowego poprzez utworzenie dla niego ObjectWrapperFactory
#define CORE_PLUGIN_ADD_OBJECT_WRAPPER(className)               \
{																		\
	try { plugin->addObjectWrapperPrototype<className>(); }		\
	catch(std::exception & e) { PLUGIN_LOG_DEBUG("Error while loading Object Wrapper for class " << #className << ": " << e.what()); } \
	catch(...) { PLUGIN_LOG_DEBUG("UNKNOWN error while loading Object Wrapper for class " << #className); } \
}     


////////////////////////////////////////////////////////////////////////////////
namespace core {
	////////////////////////////////////////////////////////////////////////////////

//! Interfejs pluginu przez który dostarczane są usługi (serwisy) i prototypy elementów przetwarzających dane
class IPlugin : public plugin::IIdentifiable, public plugin::IPluginDescription
{
public:
	//! Typ funkcji inicjującej plugin
	typedef bool(*initializeFunc)();
	//! Typ funkcji deinicjującej plugin
	typedef void(*deinitializeFunc)();

public:
	//! Pusty destruktor wirtualny
	virtual ~IPlugin() {}

	//! \param name Nazwa pluginu
	virtual void setName(const std::string & name) = 0;
	//! \param id Identyfikator pluginu
	virtual void setID(core::UniqueID id) = 0;
	//! \param langCode Kod domyślnego języka pluginu wg ISO639
	virtual void setDefaultLanguageCode(const std::string & langCode) = 0;
	//! \param major Główna wersja pluginu
	//! \param minor Podrzędna wersja pluginu
	//! \param patch Wersja patcha pluginu
	virtual void setVersion(const core::Version::VersionNumberType major,
		const core::Version::VersionNumberType minor,
		const core::Version::VersionNumberType patch) = 0;

	//! \param init Funkcja wołana przy inicjalizacji pluginu
	//! \param lateInit Funkcja wołana przy późnej inicjalizacji pluginu
	//! \param deinit Funkcja wołana przy deinicjalizacji pluginu - odłączenie sie od wszystkich zewnętrznych zasobów
	virtual void setLoadDescription(
		initializeFunc init,
		initializeFunc lateInit,
		deinitializeFunc deinit) = 0;

	//! \param name Nazwa dostawcy
	//! \param shortName Skrócona nazwa dostawcy
	//! \param description Opis dostawcy
	//! \param contact Kontakt z dostawcą
	virtual void setVendor(const std::string & name,
		const std::string & shortName,
		const std::string & description,
		const std::string & contact) = 0;
	
	//! \param shortName Skrócona nazwa pluginu
	//! \param description Opis pluginu	
	virtual void setDescription(const std::string & name,		
		const std::string & description) = 0;

	//! \param service Serwis
	virtual void addService(plugin::IServicePtr service) = 0;
	//! \param source Źródło
	virtual void addSource(plugin::ISourcePtr source) = 0;
	//! \param visualizerPrototype Prototyp wizualizatora
	virtual void addVisualizerPrototype(plugin::IVisualizerPtr visualizerPrototype) = 0;
	//! \param visualizerPrototype Prototyp wizualizatora
	//virtual void addVisualizerActionPrototype(plugin::IVisualizer * visualizerPrototype) = 0;
	//! \param parserPrototype Prototyp parsera
	virtual void addParserPrototype(plugin::IParserPtr parserPrototype) = 0;
	//! \param objectWrapperPrototype Prototyp obiektu domenowego
	virtual void addObjectWrapperPrototype(utils::ObjectWrapperPtr objectWrapperPrototype) = 0;
	//! \tparam T Typ dla którego rejestrujemy prototyp OW
	//! \param dummy Wskaźnik nie powinien być używany
	template<class T>
	void addObjectWrapperPrototype(const T * dummy = nullptr)
	{
		addObjectWrapperPrototype(utils::ObjectWrapper::create<T>());
	}
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__
