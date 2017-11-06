/********************************************************************
	created:	2013/01/07
	created:	7:1:2013   20:43
	filename: 	DataSinkManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD___DATASINKMANAGER_H__
#define HEADER_GUARD___DATASINKMANAGER_H__


#include <utils/ObserverPattern.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/newVdf/DataSink.h>

namespace vdf {

class DataSinkManager : public IDataSinkManager
{
    friend class DataSink;
public:
    //!
    typedef std::list<DataSink*> DataSinks;

private:

    //! Prototypy źródeł danych.
    IDataSinks prototypes;
    //! Lista śledząca źródła danych.
    DataSinks dataSinks;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    DataSinkManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~DataSinkManager();

public:

    //! \param dataSink
    virtual void registerDataSink(const IDataSinkPtr & dataSink);

    //! \return Wyliczenie prototypów.
    virtual DataSinksConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototypów.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks źródła.
    inline IDataSinkConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID źródła.
    IDataSinkConstPtr getPrototype(core::UniqueID id) const;

    ////! Tworzy instancję źródła.
    ////! \param id id źródła.
    ////! \return Instancja źródła.
    //DataSinkPtr createDataSink(core::UniqueID id);

    //! \return Liczba instancji źródeł danego typu.
    int getNumInstances(core::UniqueID id);

private:
    ////! Tworzy instancję źródła.
    ////! \param id id źródła.
    ////! \return Instancja źróła.
    //DataSinkPtr createDataSink(const core::IDataSinkConstPtr& prototype);
    ////! 
    //DataSinkPtr createDataSink(const DataSink& prototype);

    //! \return Indeks prototypu.
    int getPrototypeIdx(core::UniqueID id) const;

    //void notifyCreated(DataSink* dataSink);
    //void notifyDestroyed(DataSink* dataSink);
};
typedef utils::shared_ptr<DataSinkManager> DataSinkManagerPtr;
typedef utils::shared_ptr<const DataSinkManager> DataSinkManagerConstPtr;

}

#endif  

