/********************************************************************
	created:  2014/06/05	17:05:14
	filename: HMDBSourceContextOperations.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXTOPERATIONS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXTOPERATIONS_H__

#include <corelib/IJobManager.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/hmdbCommunication/IHMDBFtp.h>
#include "ShallowCopyUtils.h"
#include <utils/Filesystem.h>
#include <corelib/IJobManager.h>
#include <hmdbserviceslib/IncrementalBranchShallowCopy.h>

namespace hmdbServices
{
	class IFileStoremanWS;
}

namespace hmdbCommunication
{

	class HMDBStorageProgress : public IHMDBStorageProgress
	{
	public:
		HMDBStorageProgress();

		virtual ~HMDBStorageProgress();

		virtual void setProgress(const float progress);

		virtual void setError(const std::string & error);

		virtual bool aborted() const;

		float progress() const;

		std::string error() const;

		void abort();

	private:
		std::string error_;
		volatile float progress_;
		volatile bool aborted_;
	};

	class HMDBCompondStorageProgress
	{
	private:

		class HMDBCompondStorageSubProgress : public IHMDBStorageProgress
		{
		public:
			HMDBCompondStorageSubProgress(HMDBCompondStorageProgress * parent);
			virtual ~HMDBCompondStorageSubProgress();

			//! \param error Opis błędu
			virtual void setError(const std::string & error);
			virtual void setProgress(const float progress);
			virtual bool aborted() const;

			float progress() const;

			std::string error() const;

		private:
			HMDBCompondStorageProgress * parent;
			std::string error_;
			volatile float progress_;
		};

	public:
		HMDBCompondStorageProgress();

		virtual ~HMDBCompondStorageProgress();

		void setSize(const unsigned int size);

		HMDBCompondStorageSubProgress * get(const unsigned int idx);

		virtual bool aborted() const;

		float progress() const;

		std::string error() const;

		void abort();

	private:
		volatile bool aborted_;
		std::vector<utils::shared_ptr<HMDBCompondStorageSubProgress>> subProgresses;
	};	

	class DownloadHelper
	{
	public:
		DownloadHelper(IHMDBStoragePtr storage, const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads);

		void download();

		void wait();

		void abort();

		float progress() const;

		void filterDownloaded();

		const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloaded() const;

		void store();

		void release();

		~DownloadHelper();

	private:
		bool released;
		HMDBCompondStorageProgress storeProgress;
		const IHMDBStoragePtr storage;
		const std::list<IHMDBRemoteContext::DownloadOperationPtr> downloads;
		std::list<IHMDBRemoteContext::DownloadOperationPtr> downloaded_;
	};


	//! Interfejs opisujący wiele operacji
	class ICompoundOperation : public IHMDBRemoteContext::IOperation
	{
	public:
		//! Polityka wykonania
		enum ExecutionPolicy
		{
			Any,			//! Pojawiające się błędy nie przeszkodzą pozostałym operacjom
			AllOrNothing	//! Pierwszy napotkany błąd przerywa pozostałe i wycofuje wszystkie zmiany
		};

		//! Sposób wykonania
		enum ExecutionSchema
		{
			Serial,		//! Zadania wykonywane sa jedno po drugim
			Parallel	//! Zadania wykonywane są równolegle
		};

	public:
		//! Destruktor wirtualny
		virtual ~ICompoundOperation() {}
		//! \return Postęp
		virtual float normalizedProgress() const
		{
			const auto s = size();
			float ret = 0.0;
			for (unsigned int i = 0; i < s; ++i){
				ret += operation(i)->normalizedProgress();
			}

			return s > 0 ? ret / (float)s : 1.0;
		}
		//! \return Ilość podoperacji
		virtual unsigned int size() const = 0;
		//! \param idx Index podoperacji
		virtual IHMDBRemoteContext::OperationConstPtr operation(const unsigned int idx) const = 0;
		//! \param idx Index podoperacji
		virtual IHMDBRemoteContext::OperationPtr operation(const unsigned int idx) = 0;
		//! \return Polityka wykonania operacji
		virtual ExecutionPolicy executionPolicy() const = 0;
		//! \return sposób wykonania operacji
		virtual ExecutionSchema executionSchema() const = 0;
	};

	//! Wskaźnik dla złożonej operacji
	typedef utils::shared_ptr<ICompoundOperation> CompoundOperationPtr;
	//! Wskaźnik dla złożonej operacji
	typedef utils::shared_ptr<const ICompoundOperation> CompoundOperationConstPtr;

	class IHMDBStatusManager;

	//! Operacja zbiorcza, wspiera rozne polityki wykonania i schematy wykonania
	class CompoundOperation : public ICompoundOperation
	{
	public:

		typedef std::vector<IHMDBRemoteContext::OperationPtr> Operations;

	public:
		//! Konstruktor
		//! \param ep Polityka wykonania
		//! \param es Schemat wykonania
		CompoundOperation(const ExecutionPolicy ep,
			const ExecutionSchema es,
			const Operations & operations = Operations());

		//! Destruktor wirtualny
		virtual ~CompoundOperation();
		//! \return Ilość podoperacji
		virtual unsigned int size() const;
		//! \param idx Index podoperacji
		virtual IHMDBRemoteContext::OperationConstPtr operation(const unsigned int idx) const;
		//! \param idx Index podoperacji
		virtual IHMDBRemoteContext::OperationPtr operation(const unsigned int idx);
		//! \return Polityka wykonania operacji
		virtual ExecutionPolicy executionPolicy() const;
		//! \return Sposób wykonania operacji
		virtual ExecutionSchema executionSchema() const;
		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda przerywa operację
		virtual void abort();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! \return Stan operacji
		virtual Status status() const;
		//! \return Opis błędu
		virtual std::string error() const;

	private:
		//! Sposoby i polityki wykonania
		void callAnySerial();
		void callAnyParallel();
		void callAllSerial();
		void callAllParallel();
		//! Metoda dla obserwacji
		void observe();

		Status compoundStatus() const;
		IHMDBRemoteContext::OperationPtr findErrorOperation() const;

	private:
		//! Operacje wchodzacje w skład operacji zbiorczej
		Operations operations;
		//! Polityka wykonania
		const ExecutionPolicy ep;
		//! Schemat wykonania
		const ExecutionSchema es;
		//! Wątek realizujacy operacje
		core::Thread workerThread;
		//! Operacja która spowodowała błąd
		mutable IHMDBRemoteContext::OperationPtr errorOperation;
		//! Status
		mutable volatile Status status_;
		//! Job
		core::Job<void> job;
	};

	//! Klasa realizująca upload danych, dostarcza informacji o identyfikatorze pliku po zapisie w bazie
	class UploadOperation : public IHMDBRemoteContext::IUploadOperation
	{
	public:
		//! Funktor realizujący zapis pliku do bazy danych
		//! Zwraca identyfikator pliku
		typedef std::function < int(const std::string & file,
			const std::string & fileDescription,
			const std::string & subdirectory) > HMDBStore;

	public:
		//! Konstruktor
		//! \param fileName Nazwa pliku na ftp
		//! \param source Źródło pliku
		//! \param ftp Połączenie uzywane do wysłania danych
		//! \param store Usługa HMDB zapisująca plik w bazie
		//! \param fileDescription Opis pliku
		//! \param subdirectory
		UploadOperation(const std::string & fileName,
			IHMDBFtp::TransferPtr transfer,
			HMDBStore store,
			const std::string & fileDescription = std::string(),
			const std::string & subdirectory = std::string());

		//! Destruktor wirtualny
		virtual ~UploadOperation();
		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! Metoda anuluje aktualne zadanie
		virtual void abort();
		//! \return Stan operacji
		virtual Status status() const;
		//! \return Opis błędu
		virtual std::string error() const;
		//! \return Psotep operacji
		virtual float normalizedProgress() const;
		//! \return Identyfikator pliku po zapisie w bazie
		virtual hmdbServices::ID fileID() const;

	private:
		//! Uploaduje plik
		void upload();

	private:
		//! Status
		volatile Status status_;
		//! Postęp
		volatile float progress_;
		//! ID pliku po upload
		volatile hmdbServices::ID fileID_;
		//! Nazwa pliku w bazie danych
		const std::string fileName;
		//! Opis pliku
		const std::string fileDescription;
		//! Podkatalog pliku
		const std::string subdirectory;
		//! Operacja transferu
		IHMDBFtp::TransferPtr transfer;
		//! Operacja zapisująca plik do bazy
		HMDBStore store;

		core::Job<void> job;
	};

	//! Interfejs zadania dostarczającego mapy plików do ściągnięcia
	class PrepareHMDB
	{
	public:
		//! \param session Sesja usług HMDB
		//! \param toPrepare Pliki do przygotowania do ściągnięcia po stronie HMDB
		//! \param Polityka wykonania
		PrepareHMDB(hmdbServices::IFileStoremanWS * fs,
			const hmdbServices::ID fileID,
			const hmdbServices::DateTime & since = hmdbServices::DateTime::now());

		//! Destruktor wirtualny
		virtual ~PrepareHMDB();

		//! \return Mapa przygotowanych plików do ściągnięcia
		std::string prepareFilePath();

		//! \return Czyści baze danych ruchu
		void clearHMDB();

	private:
		hmdbServices::IFileStoremanWS * fs;
		const hmdbServices::ID fileID_;
		std::string preparedFilePath_;
		const hmdbServices::DateTime since;
	};

	class ITransferIO
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITransferIO() {}

		//! \return Wyjscia dla sciaganych plikow
		virtual IHMDBStorageOperations::OStreamPtr prepareOutput() = 0;
		//! Metoda zamyka zasoby użyte do zapisu danych przychodzących
		virtual void closeOutput() = 0;

		//! \return Otiera nasz output do czytania
		virtual IHMDBStorageOperations::IStreamPtr openInput() = 0;
		//! Metoda zamyka otwarty do czytania output i zwalnia wszystkie jego zasoby
		virtual void closeInput() = 0;
		//! Metoda zwalnia wszystkie zasoby niezależnie czy to input czy output
		virtual void release() = 0;
	};

	class TmpFileTransferIO : public ITransferIO
	{
	public:
		//! Domyslny konstruktor
		TmpFileTransferIO();
		//! Destruktor wirtualny
		virtual ~TmpFileTransferIO();
		//! \return Wyjscia dla sciaganych plikow
		virtual IHMDBStorageOperations::OStreamPtr prepareOutput();
		//! Metoda zamyka zasoby użyte do zapisu danych przychodzących
		virtual void closeOutput();

		//! \return Otiera nasz output do czytania
		virtual IHMDBStorageOperations::IStreamPtr openInput();
		//! Metoda zamyka otwarty do czytania output i zwalnia wszystkie jego zasoby
		virtual void closeInput();
		//! Metoda zwalnia wszystkie zasoby niezależnie czy to input czy output
		virtual void release();

	private:
		//! Strumień do zapisu danych
		utils::shared_ptr<std::fstream> stream;
		//! Ściezka pliku
		utils::Filesystem::Path tmpFilePath;
	};

	class MemoryTransferIO : public ITransferIO		
	{
	public:
		//! Konstruktor domyslny
		MemoryTransferIO();
		//! Destruktor wirtualny
		virtual ~MemoryTransferIO();
		//! \return Wyjscia dla sciaganych plikow
		virtual IHMDBStorageOperations::OStreamPtr prepareOutput();
		//! Metoda zamyka zasoby użyte do zapisu danych przychodzących
		virtual void closeOutput();

		//! \return Otiera nasz output do czytania
		virtual IHMDBStorageOperations::IStreamPtr openInput();
		//! Metoda zamyka otwarty do czytania output i zwalnia wszystkie jego zasoby
		virtual void closeInput();
		//! Metoda zwalnia wszystkie zasoby niezależnie czy to input czy output
		virtual void release();

	private:
		//! Strumień z danymi
		IHMDBStorageOperations::IOStreamPtr stream;
	};

	//! Operacja pozwalajaca realizowac sciaganie plikow z HMDB
	class FileDownload : public IHMDBRemoteContext::IDownloadOperation
	{
	public:
		//! \param fileToDownload		
		//! \param ptOp
		//! \param storage
		FileDownload(const IHMDBRemoteContext::FileDescriptor & fileToDownload,
			utils::shared_ptr<PrepareHMDB> prepareHMDB,
			utils::shared_ptr<ITransferIO> transferIO,
			IHMDBFtp * ftp);

		//! Destruktor wirtualny
		virtual ~FileDownload();

		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! Metoda anuluje aktualne zadanie
		virtual void abort();
		//! \return Stan operacji
		virtual Status status() const;
		//! \return Opis błędu
		virtual std::string error() const;
		//! \return Psotep operacji
		virtual float normalizedProgress() const;
		//! \return Identyfikator ściąganego pliku
		virtual IHMDBRemoteContext::FileDescriptor fileID() const;
		//! \return Czy plik jest dostepny w storage (ściągnięto i poprawnie zapisano, choć reszta operacji mogła pójsć nie tak)
		virtual bool fileDownloaded() const;
		//! \return Strumień ściągniętych danych
		virtual IHMDBStorageOperations::IStreamPtr stream() const;
		//! Metoda czyści zapisane dane
		virtual void release();

	private:
		//! Wykonanie operacji
		void download();

	private:		
		//! Identyfikator ściąganego pliku
		const IHMDBRemoteContext::FileDescriptor fileID_;
		//! Czy plik został ściągnięty - można próbowac wyciągnąc go ze storage?
		volatile bool downloaded_;
		//! Czy plik został ściągnięty - można próbowac wyciągnąc go ze storage?
		volatile int progress_;
		//! Status ściągania
		volatile Status status_;
		//! Operacja przygotowujaca pliki w HMDB
		utils::shared_ptr<PrepareHMDB> prepareHMDB;
		//! Operacja przygotowujaca output dla sciaganego pliku
		utils::shared_ptr<ITransferIO> transferIO;
		//! FTP z którego będziemy ściągać
		IHMDBFtp * ftp;
		//! Operacja sciagajaca przygotowany plik z FTP
		IHMDBFtp::TransferPtr transfer;

		core::Job<void> job;
	};

	class MultipleFilesDownloadAndStore : public IHMDBRemoteContext::IOperation
	{
	public:
		//! \param downloads Operacje ściągania które biora udzuał w synchronizacji
		//! \param storage Storage do którego ewentualnie zapisze dane
		MultipleFilesDownloadAndStore(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
			IHMDBStoragePtr storage);
		//! Destruktor wirtualny
		virtual ~MultipleFilesDownloadAndStore();
		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! Metoda anuluje aktualne zadanie
		virtual void abort();
		//! \return Stan operacji
		virtual Status status() const;
		//! \return Opis błędu
		virtual std::string error() const;
		//! \return Psotep operacji
		virtual float normalizedProgress() const;

	private:
		//! Faktyczna operacja ściągania uruchamiana w wątku
		void download();
		//! \param finishedDownloads Ściągania plików zakończone powodzeniem
		virtual void downloadFinished(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & finishedDownloads) {}

	private:
		std::string error_;
		DownloadHelper downloadHelper;
		volatile Status status_;
		core::Job<void> job;
	};

	class MultipleFilesDownloadStoreAndStatusUpdate : public MultipleFilesDownloadAndStore
	{
	public:
		MultipleFilesDownloadStoreAndStatusUpdate(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
			IHMDBStoragePtr storage, IHMDBStatusManagerPtr statusManager, ShallowCopyConstPtr shallowCopy);

		virtual ~MultipleFilesDownloadStoreAndStatusUpdate();

	private:
		//! \param finishedDownloads Ściągania plików zakończone powodzeniem
		virtual void downloadFinished(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & finishedDownloads);

	private:
		IHMDBStatusManagerPtr statusManager;
		ShallowCopyConstPtr shallowCopy;
	};

	class SynchronizeOperationImpl : public MultipleFilesDownloadAndStore
	{
	public:
		SynchronizeOperationImpl(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
			IHMDBStoragePtr storage);

		~SynchronizeOperationImpl();

		virtual void abort();

		virtual float normalizedProgress() const;

		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		ShallowCopyConstPtr shallowCopy() const;
		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const;

	private:

		//! \param finishedDownloads Ściągania plików zakończone powodzeniem
		virtual void downloadFinished(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & finishedDownloads);

	private:
		//! Pełna płytka kopia bazy danych
		ShallowCopyConstPtr shallowCopy_;
		//! Przyrostowa kopia bazy danych
		IncrementalBranchShallowCopyConstPtr incrementalShallowCopy_;
		//! Postęp rozpakowywania
		HMDBStorageProgress extractProgress;
	};

	class ExtractShallowcopy
	{
	public:
		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		ShallowCopyConstPtr shallowCopy() const;
		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const;	

		void extract(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads, IHMDBStorageProgress * progress);

	private:

		ShallowCopyPtr shallowCopy_;
		IncrementalBranchShallowCopyConstPtr incrementalShallowCopy_;
	};

	class SynchronizeOperation : public IHMDBShallowCopyRemoteContext::ISynchronizeOperation
	{
	public:
		//! \param downloads Operacje ściągania które biora udzuał w synchronizacji
		//! \param storage Storage do którego ewentualnie zapisze dane
		SynchronizeOperation(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
			IHMDBStoragePtr storage);
		//! Destruktor wirtualny
		virtual ~SynchronizeOperation();
		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! Metoda anuluje aktualne zadanie
		virtual void abort();
		//! \return Stan operacji
		virtual Status status() const;
		//! \return Opis błędu
		virtual std::string error() const;
		//! \return Psotep operacji
		virtual float normalizedProgress() const;

		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		virtual ShallowCopyConstPtr shallowCopy() const;
		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		virtual IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const;

	private:
		//! Postęp wypakowywania płytkich kopii
		SynchronizeOperationImpl op;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXTOPERATIONS_H__
