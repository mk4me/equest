/********************************************************************
	created:  2014/06/03	13:48:46
	filename: SQLCipherStorage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__

#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <utils/Filesystem.h>
#include <mutex>
#include <sqlite3.h>

namespace hmdbCommunication
{
	class SQLCipherStorage : public IHMDBStorage
	{
	public:

		struct SharedState
		{
			//! Klucz bazy
			std::string key_;
			//! Œcie¿ka do pliku z baz¹
			utils::Filesystem::Path path_;
		};

		typedef threadingUtils::TransactionSharedState<SharedState> TransactionSharedState;
		typedef threadingUtils::TransactionImplHelper<IHMDBStorage::transaction_type, TransactionSharedState>::SharedStatePtr SharedStatePtr;

	public:
		//! Konstruktor domyœlny
		SQLCipherStorage();
		//! Destruktor wirtualny
		virtual ~SQLCipherStorage();
		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym zaszyfrowana jest baza
		void open(const utils::Filesystem::Path & path,
			const std::string & key);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym zaszyfrowana jest baza
		//! \return Czy uda³o siê stworzyæ bazê
		static const bool create(const utils::Filesystem::Path & path,
			const std::string & key);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param oldKey Aktualny klucz, którym zaszyfrowana jest baza
		//! \param newKey Klucz, którym baza zostanie zaszyfrowana
		//! \return Czy uda³o siê zmieniæ klucz
		static const bool changeKey(const utils::Filesystem::Path & path,
			const std::string & oldKey,	const std::string & newKey);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym baza zostanie zaszyfrowana
		//! \return Czy uda³o siê ustawiæ klucz
		static const bool setKey(const utils::Filesystem::Path & path,
			const std::string & key);

		//! \param path Œcie¿ka do pliku z baz¹
		//! \param key Klucz, którym zaszyfrowana jest baza
		//! \return Czy baza ma poprawny format
		static const bool verify(const utils::Filesystem::Path & path,
			const std::string & key);

		//! \return Nazwa storage
		virtual const std::string name() const override;
		//! \return Nazwa protoko³u (http, file, ftp, ...)
		virtual const std::string protocol() const override;
		//! \return Czy sotrage otwarty do dzia³ania
		virtual const bool isOpened() const override;
		//! Metoda powinna zamykaæ storage, zapisuj¹c wszystkie jeszcze niezapisane dane
		//! Po tym wywo³aniu isOpened powinno zwracaæ false!!
		virtual void close() override;
		//! \return Transakcja
		virtual TransactionPtr transaction() override;
		//! \return Transakcja
		virtual TransactionConstPtr transaction() const override;

	private:
		//! Wspóldzielony stan transakcji
		SharedStatePtr sharedState;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__SQLCIPHERSTORAGE_H__