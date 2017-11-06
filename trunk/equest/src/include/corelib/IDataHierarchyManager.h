/********************************************************************
	created:  2015/07/20	12:53:56
	filename: IDataHierarchyManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___IDATAHIERARCHYMANAGER_H__
#define __HEADER_GUARD___IDATAHIERARCHYMANAGER_H__

#include <corelib/ITransaction.h>
#include <corelib/IDataHierarchyManagerReader.h>

namespace core
{
	class IDataHierarchyManager : public IDataHierarchyManagerReader
	{
	public:

		//! Operacje zwi¹zane z zarz¹dzaniem hierarchicznymi danymi
		class IOperations : public IDataHierarchyManagerReader::IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			//! Dodanie roota, odczytanie wszystkich danych w hierarchii i dodanie ich do DM
			//! \param ptr dodawany element
			virtual void addRoot(IHierarchyItemConstPtr ptr) = 0;
			//! Usuniêcie roota z DM
			//! \param ptr element do usuniêcia, musi byæ wczeœniej dodany
			virtual void removeRoot(IHierarchyItemConstPtr ptr) = 0;
			//! Wywo³ane, gdy hierarchia, lub jej dane siê zmieni³y
			//! \param ptr update'owany element
			virtual void updateRoot(IHierarchyItemConstPtr ptr) = 0;
		};

		class ITransaction : public core::ITransaction, public IOperations
		{
		public:
			virtual ~ITransaction() {}
		};

		//! Typ transakcji dodaj¹cej dane w postaci hierarchicznej
		typedef utils::shared_ptr<ITransaction> TransactionPtr;

	public:
		virtual ~IDataHierarchyManager() {}

		virtual TransactionPtr transaction() = 0;
	};
}

#endif	// __HEADER_GUARD___IDATAHIERARCHYMANAGER_H__