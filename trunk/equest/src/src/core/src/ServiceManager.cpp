#include "CorePCH.h"
#include "ServiceManager.h"
#include "ApplicationCommon.h"
#include <loglib/Exceptions.h>

namespace core {

ServiceManager::ServiceManager()
{
}


ServiceManager::~ServiceManager(void)
{

}

void ServiceManager::update(double deltaTime)
{
    for(auto it = servicesList.begin(); it != servicesList.end(); ++it){
        try{
            (*it)->update(deltaTime);
        }catch(std::exception & e){
            CORE_LOG_ERROR("Service: " << (*it)->name() << " ID: " << (*it)->ID() << " caused an error during update: " << e.what());
        }catch(...){
            CORE_LOG_ERROR("Service: " << (*it)->name() << " ID: " << (*it)->ID() << " caused an UNKNOWN error during update");
        }
    }
}

void ServiceManager::finalizeServices()
{
    for(auto it = servicesList.begin(); it != servicesList.end(); ++it){
        try{
			CORE_LOG_DEBUG("ServiceManager: finalizing " << (*it)->name() << " service");
            (*it)->finalize();
            CORE_LOG_DEBUG("ServiceManager: finalized correctly " << (*it)->name() << " service");
        }
        catch(loglib::runtime_error & e){
            CORE_LOG_ERROR("ServiceManager: Error finalizing " << (*it)->name() << " service with error " << e.what());
        }
        catch(std::invalid_argument & e){
            CORE_LOG_ERROR("ServiceManager: Error finalizing " << (*it)->name() << " service with error " << e.what());
        }
        catch(std::exception & e){
            CORE_LOG_ERROR("ServiceManager: Error finalizing " << (*it)->name() << " service with error " << e.what());
        }
        catch(...){
            CORE_LOG_ERROR("ServiceManager: Unknown error");
        }
    }
}

void ServiceManager::registerService(plugin::IServicePtr service)
{
    if (servicesMap.find(service->ID()) == servicesMap.end()) {
        servicesMap.insert( std::make_pair(service->ID(), service));
        servicesList.push_back(service);
        CORE_LOG_INFO("Service " << service->name() << " registered.");
    } else {
        throw loglib::runtime_error("Service with this ID already registered.");
    }
};

int ServiceManager::getNumServices() const
{
    return static_cast<int>(servicesList.size());
}

plugin::IServicePtr ServiceManager::getService( int idx )
{
    if ( idx < static_cast<int>(servicesList.size()) ) {
        return servicesList[idx];
    } else {
        return plugin::IServicePtr();
    }
}

plugin::IServicePtr ServiceManager::getService( UniqueID id )
{
    ServicesMap::iterator it = servicesMap.find(id);
    if (it != servicesMap.end()) {
        return it->second;
    } else {
        return plugin::IServicePtr();
    }
}

}
