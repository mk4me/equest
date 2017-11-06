/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   22:51
    filename: IFileManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILEMANAGERREADER_H__
#define HEADER_GUARD___IFILEMANAGERREADER_H__

#include <utils/Filesystem.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>

namespace core {

	//! Zapewnia dostêp do danych plikowych aplikacji, pozwala obserwowaæ zmiany z nimi zwi¹zane
	class IFileManagerReader
	{
	public:

		class IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			//! \param files Zbiór plików ktrymi aktualnie zarz¹dza ten DataManager
			virtual void getFiles(utils::Filesystem::FilesSet & files) const = 0;

			//! \param file Plik kótry weryfikujemy czy jest zarz¹dzany przez DM
			//! \return Prawda jeœli plik jest zarz¹dzany przez ten DM
			virtual const bool isManaged(const utils::Filesystem::Path & file) const = 0;

			//! \param file Plik kótry weryfikujemy czy jest w pe³ni za³adowany
			//! \return Prawda jeœli plik jest w pe³ni za³adowany
			virtual const bool isLoadedCompleately(const utils::Filesystem::Path & file) const = 0;

			//! \param file Plik dla któego pobieramy dane
			//! \param objects [out] Lista danych dla pliku
			virtual void getObjects(const utils::Filesystem::Path & file, ConstVariantsList & objects) const = 0;

			//! \param file Plik dla któego pobieramy dane
			//! \param objects [out] Kolekcja danych pliku
			virtual void getObjects(const utils::Filesystem::Path & file, VariantsCollection & objects) const = 0;
		};

		//! Typ operacji na plikach
		enum ModificationType {
			ADD_FILE,				//! Operacja dodawania pliku
			REMOVE_FILE,			//! Operacja usuniêcia pliku	
			RELOAD_FILE,			//! Operacja prze³adowania pliku (ca³ego lub dodania tylko brakujacych danych wy³adowanych z poziomu MemoryDM)
			REMOVE_UNUSED_FILE		//! Operacja usuniêcia pliku w wyniku usuniêcia jego wszystkich obiektów ( w wyniku niepomyœlnych inicjalizacji, innych operacji u¿ytkowników na MemoryDataManager)			
		};

		//! Obiekt opisuj¹cy zmianê w DM
		struct Change
		{
			utils::Filesystem::Path filePath;		//! Plik który zmieniamy
			ModificationType modyfication;	//! Typ zmiany na plikach
			ConstVariantsList previousData;
			ConstVariantsList currentData;
		};

		//! Agregat zmian w DM
		typedef std::list<Change> ChangeList;

		//! Interfejs obserwatora zmian
		typedef IChangesObserver<ChangeList> IObserver;

		//! WskaŸnik na obiek obserwuj¹cy zmiany
		typedef utils::shared_ptr<IObserver> ObserverPtr;
	
		//! Typ wskaŸnika do sta³ej transakcji
		typedef utils::shared_ptr<const IOperations> TransactionConstPtr;			

	public:

		//! Destrutkor wirtualny
		virtual ~IFileManagerReader() {}

		//! \param fileWatcher Dodawany obserwator managera plików
		virtual void addObserver(const ObserverPtr & fileWatcher) = 0;
		//! \param fileWatcher Usuwany obserwator managera plików
		virtual void removeObserver(const ObserverPtr & fileWatcher) = 0;
		//! \return Transakcja
		virtual const TransactionConstPtr transaction() const = 0;
	};

}

#endif	//	HEADER_GUARD___IFILEMANAGERREADER_H__
