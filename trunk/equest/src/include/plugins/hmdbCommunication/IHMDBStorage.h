/********************************************************************
	created:  2014/06/03	13:37:35
	filename: IHMDBStorage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTORAGE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTORAGE_H__

#include <string>
#include <set>
#include <threadingUtils/IProgress.h>
#include <threadingUtils/ITTransaction.h>
#include <threadingUtils/SmartPtr.h>
#include <utils/Filesystem.h>
#include <networkUtils/CURLFTPHelper.h>

namespace hmdbCommunication
{

	class IHMDBStorageProgress : public threadingUtils::IProgressSupervisor
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStorageProgress() {}
		//! \param error Opis b³êdu
		virtual void setError(const std::string & error) = 0;
	};

	class IHMDBStorageOperations
	{
	public:
		//! Klucze
		typedef std::set<std::string> Keys;
		//!
		typedef utils::shared_ptr<std::istream> IStreamPtr;
		//!
		typedef utils::shared_ptr<std::iostream> IOStreamPtr;
		//!
		typedef utils::shared_ptr<std::ostream> OStreamPtr;
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStorageOperations() {}
		//! \param key
		//! \return Czy klucz wystêpuje w storage
		virtual const bool exists(const std::string & key) const = 0;
		//! \param key Klucz o który pytamy
		//! \return WskaŸnik do strumienia pozwalaj¹cego czytaæ i zapisywaæ dane spod klucza lub nullptr jesli klucza nie ma
		virtual const IOStreamPtr get(const std::string & key) = 0;
		//! \param key Klucz o który pytamy
		//! \return WskaŸnik do strumienia pozwalaj¹cego czytaæ dane spod klucza lub nullptr jesli klucza nie ma
		virtual const IStreamPtr get(const std::string & key) const = 0;
		//! Metoda nadpisuje dane jeœli ju¿ wystêpuj¹
		//! \param key Klucz pod którym zapisujemy wartoœæ
		//! \param input Dane do zapisania
		//! \return Czy uda³o siê zapisaæ poprawnie dane
		virtual const bool set(const std::string & key, IStreamPtr input) = 0;
		//! Metoda nadpisuje dane jeœli ju¿ wystêpuj¹
		//! \param key Klucz pod którym zapisujemy wartoœæ
		//! \param input Dane do zapisania
		//! \param progress Obiekt steruj¹cy postêpem zapisu		
		//! \param div Dzielnik dla progresu, jesli zapis czêœci¹ wiêkszej transakcji
		virtual void set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress) = 0;
		//! \param key Klucz który usuwam
		virtual const bool remove(const std::string & key) = 0;
		//! \param oldKey Klucz któremu zmieniamy nazwê
		//! \param newKey Nowa nazwa klucza (unikalna)
		//! \param overwrite Czy nadpisaæ jeœli istnieje ju¿ taki klucz
		//! \return Czy operacja siê powiod³a
		virtual const bool rename(const std::string & oldKey,
			const std::string & newKey, const bool overwrite = false) = 0;
		//! \return Lista kluczy w storage
		virtual const Keys keys() const = 0;
		//! \param size Iloœæ bajtów do zapisu
		//! \return Czy storage bêdzie jeszcze w stanie zapisaæ tak¹ iloœæ danych
		virtual const bool canStore(const unsigned long long size) const = 0;
		//! \param path Œcie¿ka
		//! \return Czy dana œcie¿ka dzieli przestrzeñ dyskow¹ z naszym storage
		virtual const bool shareDiskSpace(const utils::Filesystem::Path & path) const { return false; }
	};

	class IHMDBStorage : public threadingUtils::ITTransaction<IHMDBStorageOperations, threadingUtils::shared_ptr>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBStorage() {}
		//! \return Nazwa storage
		virtual const std::string name() const = 0;
		//! \return Nazwa protoko³u (http, file, ftp, ...)
		virtual const std::string protocol() const = 0;

		//! \return Czy sotrage otwarty do dzia³ania
		virtual const bool isOpened() const = 0;
		
		//! Metoda powinna zamykaæ storage, zapisuj¹c wszystkie jeszcze niezapisane dane
		//! Po tym wywo³aniu isOpened powinno zwracaæ false!!
		virtual void close() = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBStorage);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSTORAGE_H__