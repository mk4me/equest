/********************************************************************
	created:  2015/07/20	12:50:08
	filename: IDataHierarchyManagerReader.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__
#define __HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__

#include <set>
#include <corelib/IHierarchyItem.h>
#include <corelib/BaseDataTypes.h>

namespace core
{
	class IDataHierarchyManagerReader
	{
	public:		

		//! Typ zmian danych w managerze
		enum ModificationType {
			ADD_ITEM,		//! Dodajemy obiekt do DM
			REMOVE_ITEM,	//! Usuwamy obiekt z DM
			UPDATE_ITEM	//! Aktualizujemy obiekt w DM
		};

		//! Obiekt opisujący zmianę w DM
		struct Change
		{
			IHierarchyItemConstPtr item;
			ModificationType modification;			//! Typ zmiany na hierarchii
		};

		typedef std::set<IHierarchyItemConstPtr>::const_iterator ConstIterator;
		//! Agregat zmian w DM
		typedef std::list<Change> ChangeList;
		typedef IChangesObserver<ChangeList> IObserver;
		//! Wskaźnik na obiek obserwujący zmiany
		typedef utils::shared_ptr<IObserver> ObserverPtr;

		//! Operacje związane z zarządzaniem hierarchicznymi danymi
		class IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			virtual ConstIterator hierarchyBegin() const = 0;
			virtual ConstIterator hierarchyEnd() const = 0;
		};

		//! Typ transakcji dodającej dane w postaci hierarchicznej
		typedef utils::shared_ptr<IOperations> TransactionPtr;

	public:
		virtual ~IDataHierarchyManagerReader() {}

		virtual TransactionPtr transaction() const = 0;

		//! Dodaje obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void addObserver(const ObserverPtr & hierarchyWatcher) = 0;
		//! Usuwa obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void removeObserver(const ObserverPtr & hierarchyWatcher) = 0;
	};
}

#endif	// __HEADER_GUARD___IDATAHIERARCHYMANAGERREADER_H__
