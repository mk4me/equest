/********************************************************************
	created:  2014/07/25	13:06:13
	filename: IHMDBShallowCopyContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__

#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <corelib/IStreamDataManager.h>
#include <list>

namespace hmdbCommunication
{
	class IHMDBStatusManager;
	class IHMDBLocalContext;
	class IHMDBRemoteContext;

	class IHMDBShallowCopyDataContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyDataContext() {}
		//! \return Aktualna p³ytka kopia bazy danych
		virtual ShallowCopyConstPtr shallowCopy() const = 0;
		//! \param shallowCopy P³ytka kopia bazy danych
		virtual void setShallowCopy(const ShallowCopyConstPtr shallowCopy) = 0;
		//! \return Manager statusów
		virtual IHMDBStatusManagerConstPtr dataStatusManager() const = 0;
		//! \return Manager statusów
		virtual IHMDBStatusManagerPtr dataStatusManager() = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyDataContext);

	class IHMDBShallowCopyLocalContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyLocalContext() {}
		//! \param type Typ danych o które pytamy
		//! \param id Identyfikator danych
		//! \param recursive Czy pobieramy dane w dó³
		//! \return Dane domenowe za³adowane dla tego typu
		virtual core::ConstVariantsList data(const DataType type,
			const hmdbServices::ID id, const bool recursive = true) const = 0;
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param ids Identyfikatory danych		
		//! \return Czy uda³o siê za³adowaæ
		virtual bool load(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda ³aduje wszystkie dane
		virtual bool loadAll() = 0;

		//! \param type Typ danych które chcemy wy³adowaæ
		//! \param id Identyfikator danych		
		//! \return Czy uda³o siê wy³adowaæ
		virtual bool unload(const DataType type,
			const hmdbServices::ID id) = 0;

		//! Metoda wy³adowuje wszystkie dane
		virtual bool unloadAll() = 0;

		//! \param type Typ danych które chcemy wy³adowaæ
		//! \param id Identyfikator danych		
		//! \return Czy dane za³adwoane
		/*
		virtual const bool isLoaded(const DataType type,
			const hmdbServices::ID id) const = 0;*/

		virtual IHMDBShallowCopyDataContextPtr shallowCopyContext() = 0;
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const = 0;

		virtual IHMDBLocalContextPtr localContext() = 0;
		virtual IHMDBLocalContextConstPtr localContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyLocalContext);

	class IHMDBShallowCopyRemoteContext
	{
	public:

		//! Interfejs realizuj¹cy operacjê synchronizacji perspektywy danych
		class ISynchronizeOperation : public IHMDBRemoteContext::IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~ISynchronizeOperation() {}
			//! \return Pe³na p³ytka kopia bazy danych - ca³a perspektywa dostepnych danych
			virtual ShallowCopyConstPtr shallowCopy() const = 0;
			//! \return Pe³na p³ytka kopia bazy danych - ca³a perspektywa dostepnych danych
			virtual IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const = 0;
		};

		//! WskaŸnik dla operacji synchronizacji
		typedef utils::shared_ptr<ISynchronizeOperation> SynchronizeOperationPtr;
		//! WskaŸnik dla operacji synchronizacji
		typedef utils::shared_ptr<const ISynchronizeOperation> SynchronizeOperationConstPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyRemoteContext() {}
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param id Identyfikator danych	
		//! \param recursive Czy pobieramy wszystkie dane w dó³ hierarchii
		//! \param forceDownload Czy wymuszamy œci¹ganie wszystkich danych czy tylko brakuj¹cych
		//! \return Przygotowany transfer danych
		virtual std::list<IHMDBRemoteContext::DownloadOperationPtr> prepareDownload(const DataType type,
			const hmdbServices::ID id, const bool recursive = true, const bool forceDownload = false) = 0;
		//! \return Operacja synchronizacji
		virtual SynchronizeOperationPtr prepareSynchronization(IHMDBStoragePtr storage) = 0;
		//! \param shallowCopy P³ytka kopia bazy danych
		//! \param incrementalBranchShallowCopy Przyrostowa kopia bazy danych
		virtual void synchronize(const ShallowCopyConstPtr shallowCopy,
			const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy = IncrementalBranchShallowCopyConstPtr()) = 0;

		//! \param downloadOperations Operacje œci¹gania
		virtual void synchronize(const SynchronizeOperationPtr downloadOperations) = 0;

		virtual IHMDBShallowCopyDataContextPtr shallowCopyContext() = 0;
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const = 0;

		virtual IHMDBRemoteContextPtr remoteContext() = 0;
		virtual IHMDBRemoteContextConstPtr remoteContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyRemoteContext);

	class IHMDBShallowCopyContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBShallowCopyContext() {}

		virtual IHMDBShallowCopyDataContextPtr shallowCopyDataContext() = 0;
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyDataContext() const = 0;

		virtual IHMDBShallowCopyLocalContextPtr shallowCopyLocalContext() = 0;
		virtual IHMDBShallowCopyLocalContextConstPtr shallowCopyLocalContext() const = 0;

		virtual IHMDBShallowCopyRemoteContextPtr shallowCopyRemoteContext() = 0;
		virtual IHMDBShallowCopyRemoteContextConstPtr shallowCopyRemoteContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBShallowCopyContext);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSHALLOWCOPYCONTEXT_H__