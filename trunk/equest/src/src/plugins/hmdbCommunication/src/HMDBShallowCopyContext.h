/********************************************************************
	created:  2014/07/25	12:40:16
	filename: HMDBShallowCopyContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSHALLOWCOPYCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSHALLOWCOPYCONTEXT_H__

#include <utils/SmartPtr.h>
#include <corelib/IDataHierarchyManager.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/subject/Types.h>

namespace hmdbCommunication
{
	class HMDBShallowCopyDataContext : public IHMDBShallowCopyDataContext
	{
	public:
		//! Domyœlny konsturktor
		HMDBShallowCopyDataContext();
		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyDataContext();
		//! \return Aktualna p³ytka kopia bazy danych
		virtual ShallowCopyConstPtr shallowCopy() const;
		//! \return Manager statusów
		virtual IHMDBStatusManagerConstPtr dataStatusManager() const;
		//! \return Manager statusów
		virtual IHMDBStatusManagerPtr dataStatusManager();
		//! \param shallowCopy P³ytka kopia bazy danych
		virtual void setShallowCopy(const ShallowCopyConstPtr shallowCopy);

	private:
		//! P³ytka kopia bazy danych
		ShallowCopyConstPtr shallowCopy_;
		//! Manager statusów danych
		IHMDBStatusManagerPtr statusManager_;
	};

	class HMDBShallowCopyLocalContext : public IHMDBShallowCopyLocalContext
	{
	private:

		typedef std::set<hmdbServices::ID> Indexes;
		typedef std::map<hmdbServices::ID, Indexes> MotionFiles;
		typedef std::map<hmdbServices::ID, std::pair<Indexes, MotionFiles>> SessionFiles;
		typedef std::map<hmdbServices::ID, SessionFiles> SubjectFiles;

		typedef std::map<hmdbServices::ID, core::ConstVariantsList> IndexedData;
		typedef std::map<DataType, IndexedData> GroupedData;

	public:
		//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych
		//! \param localContext Lokalny kontekst
		HMDBShallowCopyLocalContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
			IHMDBLocalContextPtr localContext, core::IDataHierarchyManager * hdm);

		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyLocalContext();

		//! \param type Typ danych o które pytamy
		//! \param id Identyfikator danych
		//! \param recursive Czy pobieramy dane w dó³
		//! \return Dane domenowe za³adowane dla tego typu
		virtual core::ConstVariantsList data(const DataType type,
			const hmdbServices::ID id, const bool recursive = true) const;
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P³ytka kopia wzglêdem której dzia³amy
		//! \return Identyfikatory plików które za³adowano
		virtual bool load(const DataType type,
			const hmdbServices::ID id);

		//! Metoda ³aduje wszystkie dane
		virtual bool loadAll();

		//! \param type Typ danych które chcemy wy³adowaæ
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P³ytka kopia wzglêdem której wy³adowujemy
		//! \return Identyfikatory plików które wy³adowano
		virtual bool unload(const DataType type,
			const hmdbServices::ID id);

		//! Metoda wy³adowuje wszystkie dane
		virtual bool unloadAll();

		virtual IHMDBShallowCopyDataContextPtr shallowCopyContext();
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const;

		virtual IHMDBLocalContextPtr localContext();
		virtual IHMDBLocalContextConstPtr localContext() const;

	private:

		void updateOrAddRoot(core::IHierarchyItemConstPtr root, std::set<core::IHierarchyItemConstPtr>& roots,
			core::IDataHierarchyManager::TransactionPtr hierarchyTransaction);

		core::VariantPtr findObjectByType(const DataType type,
			const hmdbServices::ID id,
			const utils::TypeInfo & typeInfo) const;

		core::VariantPtr getSubject(const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		core::VariantPtr getSession(const std::pair<Indexes, MotionFiles> & sessionFiles,
			const IndexedData & loadedFilesData,
			PluginSubject::SubjectPtr subPtr,
			core::VariantPtr subOW,
			const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		core::VariantPtr getMotion(const Indexes & motionFiles,
			const IndexedData & loadedFilesData,
			PluginSubject::SessionPtr sPtr,
			core::VariantPtr sOW,
			const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		void removeMotion(const hmdbServices::ID id,
			const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);

		void removeSession(const hmdbServices::ID id,
			const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);

		void removeSubject(const hmdbServices::ID id,
			const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);

		SubjectFiles groupDataInHierarchy(const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);
		
		void addPatientObject(const hmdbServices::MedicalShallowCopy::Patient * patient,
			PluginSubject::SubjectID subjectID);

		void loadSubjectHierarchy(const IndexedData & loadedFilesData);

		void unloadSubjectHierarchy(const IndexedData & unloadedFiles);

	private:
		//! Manager na potrzeby hierarchi
		core::IDataHierarchyManager * hdm;
		//! Mapa danych
		mutable GroupedData data_;
		//! Kontekst p³ytkiej kopii bazy danych
		IHMDBShallowCopyDataContextPtr shallowCopyContext_;
		//! Lokalny kontekst
		IHMDBLocalContextPtr localContext_;
		//! Obs³uga hierarchi analiz
		std::map<std::set<int>, std::set<core::IHierarchyItemConstPtr>> files2roots;
		//! Obs³uga hierarchi analiz
		std::map<QString, core::IHierarchyItemConstPtr> name2root;
	};

	class HMDBShallowCopyRemoteContext : public IHMDBShallowCopyRemoteContext
	{
	public:
		//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych
		//! \param remoteContext Zdalny kontekst
		HMDBShallowCopyRemoteContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
			IHMDBRemoteContextPtr remoteContext);

		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyRemoteContext();
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param id Identyfikator danych	
		//! \param recursive Czy pobieramy wszystkie dane w dó³ hierarchii
		//! \param forceDownload Czy wymuszamy œci¹ganie wszystkich danych czy tylko brakuj¹cych
		//! \return Przygotowany transfer danych
		virtual std::list<IHMDBRemoteContext::DownloadOperationPtr> prepareDownload(const DataType type,
			const hmdbServices::ID id, const bool recursive = true, const bool forceDownload = false);
		//! \return Operacja synchronizacji
		virtual SynchronizeOperationPtr prepareSynchronization(IHMDBStoragePtr storage);
		//! \param shallowCopy P³ytka kopia bazy danych
		//! \param incrementalBranchShallowCopy Przyrostowa kopia bazy danych
		virtual void synchronize(const ShallowCopyConstPtr shallowCopy,
			const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy = IncrementalBranchShallowCopyConstPtr());

		//! \param downloadOperations Operacje œci¹gania
		virtual void synchronize(const SynchronizeOperationPtr downloadOperations);

		virtual IHMDBShallowCopyDataContextPtr shallowCopyContext();
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const;

		virtual IHMDBRemoteContextPtr remoteContext();
		virtual IHMDBRemoteContextConstPtr remoteContext() const;

	private:
		//! Kontekst p³ytkiej kopii bazy danych
		IHMDBShallowCopyDataContextPtr shallowCopyContext_;
		//! Zdalny kontekst
		IHMDBRemoteContextPtr remoteContext_;
	};

	class HMDBShallowCopyContext : public IHMDBShallowCopyContext
	{
	public:

		HMDBShallowCopyContext(IHMDBShallowCopyDataContextPtr scdc,
			IHMDBShallowCopyLocalContextPtr sclc,
			IHMDBShallowCopyRemoteContextPtr scrc = nullptr);

		virtual ~HMDBShallowCopyContext();

		virtual IHMDBShallowCopyDataContextPtr shallowCopyDataContext();
		virtual IHMDBShallowCopyDataContextConstPtr shallowCopyDataContext() const;

		virtual IHMDBShallowCopyLocalContextPtr shallowCopyLocalContext();
		virtual IHMDBShallowCopyLocalContextConstPtr shallowCopyLocalContext() const;

		virtual IHMDBShallowCopyRemoteContextPtr shallowCopyRemoteContext();
		virtual IHMDBShallowCopyRemoteContextConstPtr shallowCopyRemoteContext() const;

	private:

		IHMDBShallowCopyDataContextPtr dataContext_;
		IHMDBShallowCopyLocalContextPtr localContext_;
		IHMDBShallowCopyRemoteContextPtr remoteContext_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSHALLOWCOPYCONTEXT_H__