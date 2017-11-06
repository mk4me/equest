#include "NewVdfPCH.h"
#include <QtWidgets/QMessageBox>
#include "DataSourceManager.h"

using namespace vdf;

DataSourceManager::DataSourceManager()
{
}

DataSourceManager::~DataSourceManager()
{
    UTILS_ASSERT(dataSources.empty(), "Wszystkie źródła powinny być zniszczone.");
}

IDataSourceConstPtr DataSourceManager::getPrototype( core::UniqueID id ) const
{
    IDataSources::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataSourceConstPtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return IDataSourceConstPtr(*found);
    } else {
        return IDataSourceConstPtr();
    }
}

void DataSourceManager::registerDataSource(const IDataSourcePtr & dataProcesor )
{
    if (!getPrototype(dataProcesor->getID())) {
        prototypes.push_back(dataProcesor);
        notify();
    } else {
        throw std::runtime_error("DataSource with this ID already registered.");
    }
}

int DataSourceManager::getPrototypeIdx( core::UniqueID id ) const
{
    IDataSources::const_iterator found = std::find_if(prototypes.begin(), prototypes.end(),
        [=](const IDataSourcePtr& ptr) { return ptr->getID() == id; }
    );
    if ( found != prototypes.end() ) {
        return static_cast<int>(std::distance(prototypes.begin(), found));
    } else {
        return -1;
    }
}
