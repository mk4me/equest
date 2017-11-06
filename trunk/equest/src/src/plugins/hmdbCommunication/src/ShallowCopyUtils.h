/********************************************************************
	created:  2012/03/08
	created:  8:3:2012   11:09
	filename: ShallowCopyUtils.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYUTILS_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYUTILS_H__

#include <plugins/hmdbCommunication/Types.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <utils/Filesystem.h>

namespace hmdbCommunication
{
	class IHMDBStorageOperations;

	//! Klasa pomagająca tworzyć/zarządzać płytkimi kopiami danych
	class ShallowCopyUtils
	{
	public:

		//! Typ wuliczeniowy opisujący składowe płytkiej kopii bazy danych
		enum ShallowCopyType
		{
			Shallow = -1,				//! Perspektywa danych
			Meta = -2,					//! Metadane
			IncrementalShallow = -3,	//! Przyrostowa perspektywa danych
			IncrementalMeta = -4		//! Przyrostowe metadane
		};

		//! Identyfikator pliku płytkiej kopii bazy danych
		static const hmdbServices::ID ShallowCopyFileID = Shallow;
		//! Identyfikator pliku metadanych
		static const hmdbServices::ID MetadataFileID = Meta;
		//! Identyfikator pliku przyrostowej płytkiej kopii bazy danych
		static const hmdbServices::ID IncrementalShallowCopyFileID = IncrementalShallow;
		//! Identyfikator pliku przyrostowych metadanych
		static const hmdbServices::ID IncrementalMetadataFileID = IncrementalMeta;
		
		//! Mapa użytkowników zapisanych w Storage i ich plików płytkiej kopii bazy danych
		typedef std::map<std::string, StorageFileNames> StorageUserFiles;

	public:

		//! \param shallowCopy Płytka kopia bazy danych do wypełnienia
		//! \param motionShallowCopy Strukień płytkiej kopi danych ruchu
		//! \param motionMetadata Strumień metadanych płytkiej kopi danych ruchu
		//! \param medicalShallowCopy Strumień danych płytkiej kopi danych medycznych
		//! \param medicalMetadata Strumień metadanych płytkiej kopi danych medycznych
		static void createShallowCopy(ShallowCopy & shallowCopy,
			std::istream * motionShallowCopy, std::istream * motionMetadata,
			std::istream * medicalShallowCopy, std::istream * medicalMetadata);

		//! \param userHash Skrót użytkownika którego kopię sprawdzamy
		//! \param storage Skład z którego wyciągamy płytką kopię
		//! \return Płytka kopia bazy danych
		static ShallowCopyPtr extractShallowCopy(const std::string & userHash,
			const IHMDBStorage::TransactionConstPtr storage);

		//! \param shallowCopy Płytka kopia bazy danych
		//! \return Czy płytka kopia bazy danych jest spójna i poprawna
		static bool checkShallowCopyIntegrity(const ShallowCopy & shallowCopy);

		//! \param shallowCopy Płytka kopia bazy danych
		//! \param time Czas względem którego weryfikujemy wpłytką kopię
		//! \return Czy płytka kopia bazy danych wymaga odświeżenia (ściągnięcia nowych plików
		static bool shallowCopyRequiresRefresh(const ShallowCopy & shallowCopy,
			const hmdbServices::DateTime & time);

		//! \param dataType Typ danych dla któego chcemy wyciągnąc pliki
		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki
		//! \param recursive Czy interesują nas wszystkie pliki czy specyficzne
		//! \return Zbiór plików
		static StorageFileNames files(const DataType dataType,
			const ShallowCopy & shallowCopy, const bool recursive = true);

		//! \param dataType Typ danych dla któego chcemy wyciągnąc pliki
		//! \param id Identyfikator danych
		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki
		//! \param recursive Czy interesują nas wszystkie pliki czy specyficzne
		//! \return Zbiór plików
		static StorageFileNames files(const DataType dataType,
			const hmdbServices::ID id, const ShallowCopy & shallowCopy,
			const bool recursive = true);

		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki
		//! \return Zbiór plików
		static StorageFileNames files(const ShallowCopy & shallowCopy);

		//! \param dataType Typ danych dla któego chcemy wyciągnąc pliki
		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki		
		//! \return Zbiór plików
		static StorageFileNames extraFiles(const DataType dataType,
			const ShallowCopy & shallowCopy);

		//! \param dataType Typ danych dla któego chcemy wyciągnąc pliki
		//! \param id Identyfikator danych
		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki		
		//! \return Zbiór plików
		static StorageFileNames extraFiles(const DataType dataType,
			const hmdbServices::ID id, const ShallowCopy & shallowCopy);

		//! \param userHash Skrót użytkownika którego kopię sprawdzamy
		//! \param dataReference Referencja danych
		//! \param shallowType Typ płytkiej kopii o która pytamy
		//! \param storage Skład w którym szukamy
		//! \return Czy płytka kopia bazy danych jest dostepna w storage
		static bool shallowCopyInStorage(const std::string & userHash,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType,
			const IHMDBStorage::TransactionConstPtr storage);

		//! Usuwa wszystkie pliki płytkiej kopii bazy danych ze storage
		//! \param storage Skład w którym szukamy
		static void removeAllShallowCopies(IHMDBStorage::TransactionPtr storage);
		
		//! \param userHash Skrót użytkownika
		//! \param dataReference Referencja danych
		//! \param shallowType Typ płytkiej kopii bazy danych		
		//! \param stream Strumien do czytania danych płytkiej kopii
		//! \param storage Skład w którym zapisujemy
		static void storeShallowCopy(const std::string & userHash,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType, IHMDBStorageOperations::IStreamPtr stream,
			IHMDBStorage::TransactionPtr storage);

		//! \param userHash Skrót użytkownika
		//! \param dataReference Referencja danych
		//! \param shallowType Typ płytkiej kopii bazy danych		
		//! \param storage Skład w którym zapisujemy
		//! \return Strumien do czytania danych lub nullptr jeśli nie ma
		static IHMDBStorageOperations::IStreamPtr shallowCopyStream(const std::string & userHash,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType, const IHMDBStorage::TransactionConstPtr storage);

		//! \param userHash Skrót użytkownika
		//! \param dataReference Referencja danych
		//! \param shallowType Typ płytkiej kopii bazy danych				
		//! \return Nazwa pliku płytkiej kopii bazy danych
		static std::string shallowCopyName(const std::string & userHash,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType);

		//! \param storage Skład w którym szukamy
		//! \return Nazwy plików płytkiej kopii bazy danych
		static std::list<std::string> allShallowCopiesNames(IHMDBStorage::TransactionConstPtr storage);

		static std::list<std::string> filterShallowCopiesNames(const std::list<std::string> & src,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType);

		//! \param userHash Skrót użytkownika
		static std::list<std::string> removeUserShallowCopiesNames(const std::list<std::string> & src,
			const std::string & userHash);

		//! \param user Nazwa użytkownika
		//! \param password Hasło użytkownika
		//! \return Skrót użytkownika, unikalny dla pary user-password
		static std::string userHash(const std::string & user, const std::string & password);

		//! \param storage Miejsce składowania danych
		//! \return Lista plików płytkiej kopii bazy danych
		static StorageUserFiles groupedShallowCopiesNames(IHMDBStorage::TransactionConstPtr storage);
	};

}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYUTILS_H__
