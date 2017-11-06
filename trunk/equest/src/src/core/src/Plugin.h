/********************************************************************
    created:  2013/01/25
    created:  25:1:2013   16:22
    filename: Plugin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__PLUGIN_H__
#define HEADER_GUARD_CORE__PLUGIN_H__

#include <corelib/IPlugin.h>
#include <utils/Filesystem.h>
#include <corelib/IIdentifiable.h>

namespace core {
/**
 *	Kontener na us≥ugi.
 */
class Plugin : public IPlugin
{
public:
	//! Typ funkcji inicjujπcej elementy logiki dostarczane przez plugin do aplikacji.
	typedef void(*LoadLogicContentFunctionFunction)(IPlugin * plugin);
    //! Typ funkcji inicjujπcej plugin kontekstem aplikacji i ladowanymi modulami.
    typedef void (*InitializePluginContextFunction)(IPlugin * plugin, IApplication* coreApplication);
	//! Typ funkcji wype≥niajπcej podstawowy opis pluginu.
	typedef void (*SetPluginDescriptionFunction)(IPlugin * plugin);
    //! Typ funkcji pobierajπcej wersjÍ API z ktÛrπ budowano plugin.
    typedef int (*GetAPIVersionFunction)(int * major, int * minor, int * patch);
    //! Typ funkcji pobierajπcej typ builda pluginu
    typedef int (*GetBuildTypeFunction)();
    //! Typ funkcji pobierajπcej wersje najwaøniejszych bibliotek od ktÛrych uzaleøniony jest core
    typedef int (*GetLibrariesVersionFunction)(int* boostVersion, int* qtVersion, int* stlVersion);
    //! Typ listy us≥ug.
    typedef std::vector<plugin::IServicePtr> Services;
    //! Typ listy ürÛde≥ danych DM.
    typedef std::vector<plugin::ISourcePtr> Sources;
    //! Typ listy parserÛw.
    typedef std::vector<plugin::IParserPtr> Parsers;
    //! Typ listy wizualizatorÛw.
    typedef std::vector<plugin::IVisualizerPtr> Visualizers;
    //! Typ listy wrapperÛw.
    typedef std::vector<utils::ObjectWrapperPtr> ObjectWrapperPrototypes;

private:
    //! Lista us≥ug pluginu.
    Services services;
    //! Lista ürÛdÍ≥ danych DM pluginu.
    Sources sources;
    //! Lista parserÛw pluginu.
	Parsers parsers;
    //! Lista wizualizatorÛw pluginu.
    Visualizers visualizers;
    //! ZbiÛr wprowadzanych OW do aplikacji
    ObjectWrapperPrototypes objectWrapperPrototypes;
	//! Funkcja wo≥ana przy inicjalizacji pluginu
	initializeFunc initFunc;
	//! Funkcja wo≥ana przy pÛünej inicjalizacji pluginu
	initializeFunc lateInitFunc;
	//! Funkcja wo≥ana przy deinicjalizacji pluginu - od≥πczenie sie od wszystkich zewnÍtrznych zasobÛw
	deinitializeFunc deinitFunc;

	//! Kod jÍzyka domyslnego
	std::string defaultLanguageCode_;
    //! ID pluginu.
    UniqueID id;
    //! åcieøka do pluginu.
	utils::Filesystem::Path path;
	//! Opis pluginu
	plugin::Description desc;
	//! Dostawca pluginu
	utils::shared_ptr<plugin::VendorDescription> vendor_;
	//! Wersja pluginu
	utils::shared_ptr<Version> version_;

public:
	//! Domyúlny konstruktor
    Plugin();
	//! Destruktor wirtualny
	virtual ~Plugin();

    //! \return ID pluginu.
    const UniqueID ID() const;

    //! \return Opis pluginu
    const std::string description() const;

    //! \return Nazwa pluginu.
    const std::string name() const;
	//! \return KrÛtka nazwa pluginu.
	const std::string shortName() const;
    //! \return
	const utils::Filesystem::Path& getPath() const;

	//! \return Wersja pluginu
	const core::Version * version() const;
	//! \return Dostawca pluginu
	const plugin::VendorDescription * vendor() const;

    //! \param path
	void setPath(const utils::Filesystem::Path& path);

	//! \param name Nazwa pluginu
	virtual void setName(const std::string & name) override;

	//! \param major G≥Ûwna wersja pluginu
	//! \param minor PodrzÍdna wersja pluginu
	//! \param patch Wersja patcha pluginu
	virtual void setVersion(const core::Version::VersionNumberType major,
		const core::Version::VersionNumberType minor,
		const core::Version::VersionNumberType patch) override;

	//! \param init Funkcja wo≥ana przy inicjalizacji pluginu
	//! \param deinit Funkcja wo≥ana przy deinicjalizacji pluginu - od≥πczenie sie od wszystkich zewnÍtrznych zasobÛw
	virtual void setLoadDescription(
		initializeFunc init,
		initializeFunc lateInit,
		deinitializeFunc deinit) override;

	//! \return Czy uda≥o siÍ zainicjalizowaÊ plugin
	bool initialize();
	//! \return Czy uda≥o siÍ zainicjalizowaÊ plugin
	bool lateInitialize();
	//! Deinicjalizacja pluginu
	void deinitialize();

	//! \param name Nazwa dostawcy
	//! \param shortName SkrÛcona nazwa dostawcy
	//! \param description Opis dostawcy
	//! \param contact Kontakt z dostawcπ
	virtual void setVendor(const std::string & name,
		const std::string & shortName,
		const std::string & description,
		const std::string & contact) override;

	//! \param shortName SkrÛcona nazwa pluginu
	//! \param description Opis pluginu	
	virtual void setDescription(const std::string & name,		
		const std::string & description) override;

	//! \param langCode Kod domyúlnego jÍzyka pluginu wg ISO639
	virtual void setDefaultLanguageCode(const std::string & langCode) override;

	//! \return Kod domyúlnego jÍzyka pluginu wg ISO639
	const std::string & defaultLanguageCode() const;

	//! \param id Identyfikator pluginu
	virtual void setID(UniqueID id) override;

    //! \service Us≥uga do dodania do pluginu.
	virtual void addService(plugin::IServicePtr service) override;
    //! \return Liczba us≥ug dostarczanych przez plugin.
    int getNumServices() const;
    //! \param i
    const plugin::IServicePtr & getService(int i);

    //! \service èrÛd≥o DM do dodania do pluginu.
	virtual void addSource(plugin::ISourcePtr source) override;
    //! \return Liczba ürÛde≥ DM dostarczanych przez plugin.
    int getNumSources() const;
    //! \param i
    const plugin::ISourcePtr & getSource(int i);
	
    //! \parser Parser do dodania do pluginu.
	virtual void addParserPrototype(plugin::IParserPtr parser) override;
    //! \return Liczba parserÛw dostarczanych przez plugin.
    int getNumParsersPrototypes() const;
    //! \param i
	const plugin::IParserPtr & getParserPrototype(int i);

    //! \visualizer Visualizer do dodania do pluginu.
	virtual void addVisualizerPrototype(plugin::IVisualizerPtr visualizer) override;
    //! \return Liczba visualizerÛw dostarczanych przez plugin.
    int getNumVisualizerPrototypes() const;
    //! \param i
    const plugin::IVisualizerPtr & getVisualizerPrototype(int i);

	virtual void addObjectWrapperPrototype(utils::ObjectWrapperPtr objectWrapperPrototype) override;

    //! \return
    int getNumObjectWrapperPrototypes() const;
    //! \param i
    const utils::ObjectWrapperPtr & getObjectWrapperPrototype(int i);
	//! \return Czy plugin cokolwiek wnosi do aplikacji
	const bool empty() const;
	//! \return Czy plugin wnosi do aplikacji elementy logiki
	const bool logicEmpty() const;
};

//! Definicja wskaünika.
DEFINE_SMART_POINTERS(Plugin);

}

#endif	//	HEADER_GUARD_CORE__PLUGIN_H__
