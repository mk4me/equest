#include "CorePCH.h"
#include "Plugin.h"
#include "ApplicationCommon.h"

using namespace core;

Plugin::Plugin()
	: initFunc(nullptr), deinitFunc(nullptr)
{

}

Plugin::~Plugin()
{

}

const UniqueID Plugin::ID() const
{
	return id;
}     

const std::string Plugin::description() const
{
	return desc.description();
}

const std::string Plugin::name() const
{
	return desc.name();
}

const std::string Plugin::shortName() const
{
	return desc.shortName();
}

const utils::Filesystem::Path& Plugin::getPath() const
{ 
	return path;
}

void Plugin::setPath(const utils::Filesystem::Path& path) 
{ 
	this->path = path; 
}

void Plugin::setName(const std::string & name)
{
	desc.setName(name);
}

const core::Version * Plugin::version() const
{
	return version_.get();
}

const plugin::VendorDescription * Plugin::vendor() const
{
	return vendor_.get();
}

void Plugin::setVersion(const core::Version::VersionNumberType major,
	const core::Version::VersionNumberType minor,
	const core::Version::VersionNumberType patch)
{
	if(version_ != nullptr){
		CORE_LOG_DEBUG("Overriding plugin " << id << " -> " << desc.name() << " version");
	}

	version_.reset(new Version(major, minor, patch));
}

void Plugin::setLoadDescription(
	initializeFunc init,
	initializeFunc lateInit,
	deinitializeFunc deinit)
{
	initFunc = init;
	lateInitFunc = lateInit;
	deinitFunc = deinit;
}

bool Plugin::initialize()
{
	bool ret = true;

	if (initFunc != nullptr){
		try{
			ret = initFunc();
		}
		catch (...){
			ret = false;
		}
	}

	return ret;
}

bool Plugin::lateInitialize()
{
	bool ret = true;

	if (lateInitFunc != nullptr){
		try{
			ret = lateInitFunc();
		}
		catch (...){
			ret = false;
		}
	}

	return ret;
}

void Plugin::deinitialize()
{
	if (deinitFunc != nullptr){		
		deinitFunc();		
	}
}

void Plugin::setVendor(const std::string & name,
	const std::string & shortName,
	const std::string & description,
	const std::string & contact)
{
	if(vendor_ == nullptr){
		vendor_.reset(new plugin::VendorDescription(name, shortName, description, contact));
	}else{
		CORE_LOG_DEBUG("Overriding plugin " << id << " -> " << desc.name() << " vendor description");
		vendor_->setName(name);
		vendor_->setShortName(shortName);
		vendor_->setDescription(description);
		vendor_->setContact(contact);
	}
}
	
void Plugin::setDescription(const std::string & name,		
	const std::string & description)
{
	desc.setShortName(name);
	desc.setDescription(description);
}

void Plugin::setDefaultLanguageCode(const std::string & langCode)
{
	defaultLanguageCode_ = langCode;
}

const std::string & Plugin::defaultLanguageCode() const
{
	return defaultLanguageCode_;
}

void Plugin::setID(UniqueID id)
{
	this->id = id;
}

void Plugin::addService(plugin::IServicePtr service)
{
	services.push_back(service);
}

int Plugin::getNumServices() const
{
	return static_cast<int>(services.size());
}

const plugin::IServicePtr & Plugin::getService(int i)
{
	return services[i];
}

void Plugin::addSource(plugin::ISourcePtr source)
{
	sources.push_back(source);
}

int Plugin::getNumSources() const
{
	return static_cast<int>(sources.size());
}

const plugin::ISourcePtr & Plugin::getSource(int i)
{
	return sources[i];
}

void Plugin::addParserPrototype(plugin::IParserPtr parser)
{
	this->parsers.push_back(parser);
}

int Plugin::getNumParsersPrototypes() const
{
	return static_cast<int>(this->parsers.size());
}

const plugin::IParserPtr & Plugin::getParserPrototype(int i)
{
	return this->parsers[i];
}

void Plugin::addVisualizerPrototype(plugin::IVisualizerPtr visualizer)
{
	this->visualizers.push_back(visualizer);
}

int Plugin::getNumVisualizerPrototypes() const
{
	return static_cast<int>(this->visualizers.size());
}

const plugin::IVisualizerPtr & Plugin::getVisualizerPrototype(int i)
{
	return this->visualizers[i];
}

void Plugin::addObjectWrapperPrototype(utils::ObjectWrapperPtr objectWrapperPrototype)
{
	this->objectWrapperPrototypes.push_back(objectWrapperPrototype);
}

int Plugin::getNumObjectWrapperPrototypes() const
{
	return static_cast<int>(this->objectWrapperPrototypes.size());
}

const utils::ObjectWrapperPtr & Plugin::getObjectWrapperPrototype(int i)
{
	return this->objectWrapperPrototypes[i];
}

const bool Plugin::logicEmpty() const
{
	return services.empty() && sources.empty() && parsers.empty()
		&& visualizers.empty() && objectWrapperPrototypes.empty();
}

const bool Plugin::empty() const
{
	return (initFunc == nullptr) && (logicEmpty() == true);
}
