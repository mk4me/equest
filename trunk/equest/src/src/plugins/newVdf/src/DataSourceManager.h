/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:40
    filename: DataSourceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEMANAGER_H__
#define HEADER_GUARD___DATASOURCEMANAGER_H__

#include <utils/ObserverPattern.h>
#include <plugins/newVdf/IDataSourceManager.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/newVdf/DataSource.h>

namespace vdf {

class DataSourceManager : public IDataSourceManager
{
    friend class DataSource;
public:
    //!
    typedef std::list<DataSource*> DataSources;

private:

    //! Prototypy źródeł danych.
    IDataSources prototypes;
    //! Lista śledząca źródła danych.
    DataSources dataSources;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    DataSourceManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~DataSourceManager();

public:

    //! \param dataSource
    virtual void registerDataSource(const IDataSourcePtr & dataSource);

    //! \return Wyliczenie prototypów.
    virtual IDataSourcesConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototypów.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks źródła.
    inline IDataSourceConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID źródła.
    IDataSourceConstPtr getPrototype(core::UniqueID id) const;

    ////! Tworzy instancję źródła.
    ////! \param id id źródła.
    ////! \return Instancja źródła.
    //DataSourcePtr createDataSource(core::UniqueID id);

    //! \return Liczba instancji źródeł danego typu.
    int getNumInstances(core::UniqueID id);

private:
    ////! Tworzy instancję źródła.
    ////! \param id id źródła.
    ////! \return Instancja źróła.
    //DataSourcePtr createDataSource(const core::IDataSourceConstPtr& prototype);
    ////! 
    //DataSourcePtr createDataSource(const DataSource& prototype);

    //! \return Indeks prototypu.
    int getPrototypeIdx(core::UniqueID id) const;

    //void notifyCreated(DataSource* dataSource);
    //void notifyDestroyed(DataSource* dataSource);
};
typedef utils::shared_ptr<DataSourceManager> DataSourceManagerPtr;
typedef utils::shared_ptr<const DataSourceManager> DataSourceManagerConstPtr;

}

#endif  //  HEADER_GUARD___DATASOURCEMANAGER_H__

