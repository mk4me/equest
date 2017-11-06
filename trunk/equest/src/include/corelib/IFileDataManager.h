/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   20:10
    filename: IFileDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILEDATAMANAGER_H__
#define HEADER_GUARD___IFILEDATAMANAGER_H__

#include <corelib/ITransaction.h>
#include <corelib/IFileManagerReader.h>
#include <corelib/IDataManager.h>
#include <corelib/BaseDataTypes.h>
#include <utils/Filesystem.h>
#include <corelib/IDataManager.h>

namespace core {

	//! Interfejs dostêpu do danych plikowych i ³adowania danych w aplikacji
	class IFileDataManager : public IFileManagerReader
	{
	public:

		//! Zbiór operacji na plikach
		class IOperations : public IFileManagerReader::IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			//! \param file Plik do usuniêcia
			//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
			virtual void removeFile(const utils::Filesystem::Path & file) = 0;

			//! \param file Œcie¿ka pliku do dodania
			//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
			virtual void addFile(const utils::Filesystem::Path & file) = 0;

			//! \param file Œcie¿ka pliku do prze³adowania
			//! \param complete Czy plik ma zostaæ ca³kowicie prze³adowany czy tylko brakuj¹ce dane maj¹ zostaæ do³adowane
			virtual void reloadFile(const utils::Filesystem::Path & file,
				const bool complete = false) = 0;

			//! \param file Œcie¿ka do dodawanego pliku
			//! \return Prawda jeœli plik pomyœlnie dodano
			virtual const bool tryAddFile(const utils::Filesystem::Path & file) = 0;
			//! \param file Œcie¿ka do usuwanego pliku
			//! \return Prawda jeœli plik pomyœlnie usuniêto
			virtual const bool tryRemoveFile(const utils::Filesystem::Path & file) = 0;
			//! \param file Œcie¿ka pliku do prze³adowania
			//! \param complete Czy plik ma zostaæ ca³kowicie prze³adowany czy tylko brakuj¹ce dane maj¹ zostaæ do³adowane
			//! \return Prawda jeœli plik pomyœlnie prze³adowano
			virtual const bool tryReloadFile(const utils::Filesystem::Path & file,
				const bool complete = false) = 0;
		};

	public:
		//! Interfejs transakcji dla managera plików
		class ITransaction : public core::ITransaction, public IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITransaction() {}
		};

		//! Destruktor wirtualny
		virtual ~IFileDataManager() {}

		//! Typ transakcji - dzia³a jak RAII -> przy niszczeniu próbuje "commitowaæ" zmiany jeœli nie by³o wczeœniej ¿adnych b³êdów
		typedef utils::shared_ptr<ITransaction> TransactionPtr;
		//! \param mtransaction Transakcja MDM w ramach któej realizujemy funkcjonalnoœæ FDM
		//! \return Nowa transakcja
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEDATAMANAGER_H__
