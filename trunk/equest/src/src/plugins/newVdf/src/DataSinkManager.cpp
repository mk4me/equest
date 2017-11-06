#include "NewVdfPCH.h"
#include <QtWidgets/QMessageBox>
#include "DataSinkManager.h"

using namespace vdf;

DataSinkManager::DataSinkManager()
{
}

DataSinkManager::~DataSinkManager()
{
    UTILS_ASSERT(dataSinks.empty(), "Wszystkie źródła powinny być zniszczone.");
}

IDataSinkConstPtr DataSinkManager::getPrototype( core::UniqueID id ) const
{
    IDataSinks::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataSinkConstPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return IDataSinkConstPtr(*found);
    } else {
        return IDataSinkConstPtr();
    }
}

void DataSinkManager::registerDataSink(const IDataSinkPtr & dataProcesor )
{
    if (!getPrototype(dataProcesor->getID())) {
        prototypes.push_back(dataProcesor);
        notify();
    } else {
        throw std::runtime_error("DataSink with this ID already registered.");
    }
}

int DataSinkManager::getPrototypeIdx( core::UniqueID id ) const
{
    IDataSinks::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataSinkPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}

