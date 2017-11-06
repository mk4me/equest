/********************************************************************
	created:  2014/08/10	16:05:08
	filename: MemoryStorage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__

#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <mutex>
#include <map>

namespace hmdbCommunication
{
	class MemoryStorage : public IHMDBStorage
	{
	public:

		typedef std::map<std::string, std::string> Storage;

		typedef threadingUtils::TransactionSharedState<Storage> TransactionSharedState;
		typedef threadingUtils::TransactionImplHelper<IHMDBStorage::transaction_type, TransactionSharedState>::SharedStatePtr SharedStatePtr;

	public:
		//! Konstruktor domyœlny
		MemoryStorage();
		//! Destruktor wirtualny
		virtual ~MemoryStorage();

		//! \return Nazwa storage
		virtual const std::string name() const;
		//! \return Nazwa protoko³u (http, file, ftp, ...)
		virtual const std::string protocol() const;
		//! \return Czy sotrage otwarty do dzia³ania
		virtual const bool isOpened() const;

		//! Metoda powinna zamykaæ storage, zapisuj¹c wszystkie jeszcze niezapisane dane
		//! Po tym wywo³aniu isOpened powinno zwracaæ false!!
		virtual void close();

		//! \return Transakcja
		virtual TransactionPtr transaction();
		//! \return Transakcja
		virtual TransactionConstPtr transaction() const;

	private:
		//! Wspóldzielony stan transakcji
		SharedStatePtr sharedState;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__MEMORYSTORAGE_H__