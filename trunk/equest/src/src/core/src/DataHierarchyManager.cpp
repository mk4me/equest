#include "CorePCH.h"
#include "DataHierarchyManager.h"
#include "ApplicationCommon.h"
#include <loglib/Exceptions.h>

using namespace core;

class DataHierarchyManager::HierarchyTransaction : public IDataHierarchyManager::ITransaction
{
private:
    DataHierarchyManager* mdm;
    ChangeList modifications;
    bool transactionRollbacked;

public:
    HierarchyTransaction(DataHierarchyManager * mdm) : mdm(mdm), transactionRollbacked(false)
    {
        mdm->sync.lock();
    }

    ~HierarchyTransaction()
    {
        if(transactionRollbacked == false){
            if(modifications.empty() == false){
                mdm->updateObservers(modifications);
            }
            mdm->sync.unlock();
        }
    }

public:

    virtual const bool isRolledback() const
    {
        return transactionRollbacked;
    }

    virtual void rollback()
    {
        transactionRollbacked = true;

        // TODO
        // W obecnym modelu zarzadzania updateami nie można  ich sensownie cofnąć 

        ////cofanie pozostałych zmian od końca
        for(auto it = modifications.rbegin(); it != modifications.rend(); ++it){
            switch((*it).modification){

			case IDataHierarchyManagerReader::ADD_ITEM:
                mdm->rawRemoveRoot((*it).item);
                break;

			case IDataHierarchyManagerReader::REMOVE_ITEM:
                mdm->rawAddRoot(utils::const_pointer_cast<IHierarchyItem>((*it).item));
                break;
            }
        }

        mdm->sync.unlock();
    }

    virtual void updateRoot( IHierarchyItemConstPtr ptr )
    {
		//throw std::runtime_error("Not implemented");
        Change hc;
        hc.item = ptr;
		hc.modification = IDataHierarchyManagerReader::UPDATE_ITEM;
        modifications.push_back(hc);
    }

    virtual void addRoot( IHierarchyItemConstPtr ptr )
    {
        Change hc;
        hc.item = ptr;
		hc.modification = IDataHierarchyManagerReader::ADD_ITEM;
        modifications.push_back(hc);
        mdm->rawAddRoot(ptr);
    }

    virtual void removeRoot( IHierarchyItemConstPtr ptr )
    {
        Change hc;
        hc.item = ptr;
		hc.modification = IDataHierarchyManagerReader::REMOVE_ITEM;
        modifications.push_back(hc);
        mdm->rawRemoveRoot(ptr);
    }

	virtual ConstIterator hierarchyBegin() const
	{
		return mdm->roots.cbegin();
	}

	virtual ConstIterator hierarchyEnd() const
	{
		return mdm->roots.cend();
	}

};


IDataHierarchyManager::TransactionPtr core::DataHierarchyManager::transaction()
{
	return IDataHierarchyManager::TransactionPtr(new HierarchyTransaction(this));
}

IDataHierarchyManagerReader::TransactionPtr core::DataHierarchyManager::transaction() const
{
	return IDataHierarchyManagerReader::TransactionPtr(new HierarchyTransaction(const_cast<DataHierarchyManager*>(this)));
}

void core::DataHierarchyManager::rawAddRoot( IHierarchyItemConstPtr ptr )
{
    if (roots.find(ptr) != roots.end()) {
        throw loglib::runtime_error("HierarchyDataManager - root is already added");
    }

    roots.insert(ptr);
}

void core::DataHierarchyManager::rawRemoveRoot( IHierarchyItemConstPtr ptr )
{
    auto it = roots.find(ptr);
    if (it == roots.end()) {
        throw loglib::runtime_error("HierarchyDataManager - root was not found");
    }

    roots.erase(it);
    
}

void core::DataHierarchyManager::rawUpdateRoot( IHierarchyItemConstPtr ptr )
{
    UTILS_ASSERT(false, "NYI");
}

void core::DataHierarchyManager::addObserver( const ObserverPtr & objectWatcher )
{
    ScopedLock lock(sync);
    if(std::find(observers.begin(), observers.end(), objectWatcher) != observers.end()){
        throw loglib::runtime_error("Watcher already registered");
    }

    observers.push_back(objectWatcher);
}

void core::DataHierarchyManager::removeObserver( const ObserverPtr & objectWatcher )
{
    ScopedLock lock(sync);
    auto it = std::find(observers.begin(), observers.end(), objectWatcher);
    if(it == observers.end()){
        throw loglib::runtime_error("Watcher not registered");
    }

    observers.erase(it);
}

void core::DataHierarchyManager::updateObservers(const ChangeList & changes )
{
    for(auto it = observers.begin(); it != observers.end(); ++it){
        try{
            (*it)->observe(changes);
        }catch(...){
            //TODO
            //rozwinąć obserwatorów aby się jakoś identyfikowali!! ewentualnie robić to przez własną implementację dostarczaną konretnym obiektom
            //(osobne interfejsy readerów dla każdego elemnentu ładowanego do aplikacji - service, source, datasink, itp)
            CORE_LOG_WARNING("Error while updating memory data manager observer");
        }
    }
}