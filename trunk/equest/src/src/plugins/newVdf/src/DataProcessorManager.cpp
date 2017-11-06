#include "NewVdfPCH.h"
#include <utils/Debug.h>
#include "DataProcessorManager.h"

using namespace vdf;

DataProcessorManager::DataProcessorManager()
{
}

DataProcessorManager::~DataProcessorManager()
{
    UTILS_ASSERT(dataProcessors.empty(), "Wszystkie elementy przetwarzające powinny być zniszczone.");
    while (prototypes.size()) {
        prototypes.pop_back();
    }
 }

IDataProcessorConstPtr DataProcessorManager::getPrototype( core::UniqueID id ) const
{
    IDataProcessors::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataProcessorConstPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return IDataProcessorConstPtr(*found);
    } else {
        return IDataProcessorConstPtr();
    }
}

void DataProcessorManager::registerDataProcessor(const IDataProcessorPtr & dataProcesor )
{
    if (!getPrototype(dataProcesor->getID())) {
        prototypes.push_back(dataProcesor);
        notify();
    } else {
        throw std::runtime_error("DataProcessor with this ID already registered.");
    }
}

int DataProcessorManager::getPrototypeIdx( core::UniqueID id ) const
{
    IDataProcessors::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataProcessorPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}
