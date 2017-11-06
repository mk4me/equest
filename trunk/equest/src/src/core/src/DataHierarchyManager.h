/********************************************************************
	created:	2013/07/21
	created:	21:7:2013   12:53
	filename: 	HierarchyDataManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYDATAMANAGER_H__
#define HEADER_GUARD_CORE__HIERARCHYDATAMANAGER_H__

#include <corelib/IDataHierarchyManager.h>
#include <mutex>

namespace core {

class DataHierarchyManager : public IDataHierarchyManager
{
	typedef std::lock_guard<std::recursive_mutex> ScopedLock;

    //! Interfejs obserwatora zmian
    template <class Changes>
    class IChangesObserver
    {
    public:

        virtual ~IChangesObserver() {}

        //! \param previousValue 
        //! \param currentVal 
        //! \param type 
        //! \param modyfication 
        virtual void observe(const Changes & changes) = 0;
    };
public:
	virtual ~DataHierarchyManager() {}

public:
	virtual IDataHierarchyManager::TransactionPtr transaction();
	virtual IDataHierarchyManagerReader::TransactionPtr transaction() const;
        
    // IDataManagerReader API
    //! \param objectWatcher Obserwator DM do zarejestrowania
	virtual void addObserver(const ObserverPtr & objectWatcher);
    //! \param objectWatcher Obserwator DM do wyrejestrowania
	virtual void removeObserver(const ObserverPtr & objectWatcher);
	virtual void updateObservers(const ChangeList & changes);

private:

	void rawAddRoot(IHierarchyItemConstPtr ptr);

	void rawRemoveRoot(IHierarchyItemConstPtr ptr);

	void rawUpdateRoot(IHierarchyItemConstPtr ptr);

private:
    class HierarchyTransaction;
    friend class HierarchyTransaction;

private:
    std::set<IHierarchyItemConstPtr> roots;
    //! Obiekty obserwuj¹ce stan DM
	std::list<IDataHierarchyManagerReader::ObserverPtr> observers;
	//!
	std::recursive_mutex sync;
};
DEFINE_SMART_POINTERS(DataHierarchyManager);
}

#endif
