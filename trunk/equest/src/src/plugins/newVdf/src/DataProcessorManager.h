/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   9:12
    filename: DataProcessorManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATAPROCESSORMANAGER_H__
#define HEADER_GUARD___DATAPROCESSORMANAGER_H__

#include <corelib/BaseDataTypes.h>
#include <utils/ObserverPattern.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/DataProcessor.h>

namespace vdf {

class DataProcessorManager : public IDataProcessorManager
{
    friend class DataProcessor;
public:
    //!
    typedef std::list<DataProcessor*> DataProcessors;

private:

    //! Prototypy elementów przetwarzających.
    IDataProcessors prototypes;
    //! Lista śledząca elementy przwtwarzające.
    DataProcessors dataProcessors;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    DataProcessorManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~DataProcessorManager();

public:

    //! \param dataProcessor
    virtual void registerDataProcessor(const IDataProcessorPtr & dataProcessor);

    //! \return Wyliczenie prototypów.
    virtual IDataProcessorsConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototypów.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks elementu przetwarzającego.
    inline IDataProcessorConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID elementu przetwarzającego.
    IDataProcessorConstPtr getPrototype(core::UniqueID id) const;

    ////! Tworzy instancję elementu przetwarzającego.
    ////! \param id id elementu przetwarzającego.
    ////! \return Instancja elementu przetwarzającego.
    //DataProcessorPtr createDataProcessor(core::UniqueID id);

    //! \return Liczba instancji elementów przetwarzających danego typu.
    //int getNumInstances(core::UniqueID id);

private:
    ////! Tworzy instancję elementu przetwarzającego.
    ////! \param id id elementu przetwarzającego.
    ////! \return Instancja elementu przetwarzającego.
    //DataProcessorPtr createDataProcessor(const core::IDataProcessorConstPtr& prototype);
    ////! 
    //DataProcessorPtr createDataProcessor(const DataProcessor& prototype);

private:
    //! \return Indeks prototypu.
    int getPrototypeIdx(core::UniqueID id) const;

    //void notifyCreated(DataProcessor* dataProcessor);
    //void notifyDestroyed(DataProcessor* dataProcessor);
};
typedef utils::shared_ptr<DataProcessorManager> DataProcessorManagerPtr;
typedef utils::shared_ptr<const DataProcessorManager> DataProcessorManagerConstPtr;

}

#endif  

