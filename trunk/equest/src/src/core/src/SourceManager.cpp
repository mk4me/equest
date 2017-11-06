#include "CorePCH.h"
#include "SourceManager.h"

#include <loglib/ILog.h>
#include <corelib/PluginCommon.h>
#include "ApplicationCommon.h"
#include <loglib/Exceptions.h>

namespace core{

SourceManager::SourceManager()
{
}


SourceManager::~SourceManager(void)
{

}

void SourceManager::update(double deltaTime)
{
	for(auto it = sourcesList.begin(); it != sourcesList.end(); ++it){
		try{
			(*it)->update(deltaTime);
		}catch(std::exception & e){
			CORE_LOG_ERROR("Source: " << (*it)->name() << " ID: " << (*it)->ID() << " caused an error during update: " << e.what());
		}catch(...){
			CORE_LOG_ERROR("Source: " << (*it)->name() << " ID: " << (*it)->ID() << " caused an UNKNOWN error during update");
		}
	}
}

void SourceManager::finalizeSources()
{
	for(auto it = sourcesList.begin(); it != sourcesList.end(); ++it){
		try{
			CORE_LOG_DEBUG("SourceManager: finalizing " << (*it)->name() << " source");
			(*it)->finalize();
			CORE_LOG_DEBUG("SourceManager: finalized correctly " << (*it)->name() << " source");
		}
		catch(std::exception & e){
			CORE_LOG_ERROR("SourceManager: Error finalizing " << (*it)->name() << " source with error " << e.what());
		}
		catch(...){
			CORE_LOG_ERROR("ServiceManager: Unknown error");
		}
	}
}

void SourceManager::registerSource(const plugin::ISourcePtr & source)
{
	if (sourcesMap.find(source->ID()) == sourcesMap.end()) {
		sourcesMap.insert( std::make_pair(source->ID(), source));
		sourcesList.push_back(source);
		CORE_LOG_INFO("Source " << source->name() << " registered.");
	} else {
		throw loglib::runtime_error("Source with this ID already registered.");
	}
};

int SourceManager::getNumSources() const
{
	return static_cast<int>(sourcesList.size());
}

plugin::ISourcePtr SourceManager::getSource( int idx )
{
	if ( idx < static_cast<int>(sourcesList.size()) ) {
		return sourcesList[idx];
	} else {
		return plugin::ISourcePtr();
	}
}

plugin::ISourcePtr SourceManager::getSource( UniqueID id )
{
	auto it = sourcesMap.find(id);
	if (it != sourcesMap.end()) {
		return it->second;
	} else {
		return plugin::ISourcePtr();
	}
}

}
