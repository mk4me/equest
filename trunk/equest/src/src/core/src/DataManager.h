/********************************************************************
	created:  2013/01/08
	created:  8:1:2013   22:31
	filename: MemoryDataManager.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___MEMORYDATAMANAGER_H__
#define HEADER_GUARD___MEMORYDATAMANAGER_H__

#include <list>
#include <corelib/IDataManagerReader.h>
#include <corelib/IDataManager.h>
#include <mutex>

namespace core {
	class DataManager : public IDataManager
	{
	protected:

		class Transaction;
		friend class Transaction;

		class ReaderTransaction;
		friend class ReaderTransaction;

		//! Typ mapy obiektów.
		typedef std::map< utils::TypeInfo, VariantsSet > ObjectsByTypes;

		typedef std::recursive_mutex SyncPolicy;
		typedef std::lock_guard<SyncPolicy> ScopedLock;

	protected:

		//! Obiekty pochodz¹ce z parserów.
		ObjectsByTypes objectsByTypes;

		//! Obiekt na potrzeby synchronizacji
		mutable SyncPolicy sync;

		//! Obiekty obserwuj¹ce stan DM
		std::list<ObserverPtr> observers;

	public:

		DataManager();

		virtual ~DataManager();

	public:
		// IDataManagerReader API
		//! \param objectWatcher Obserwator DM do zarejestrowania
		virtual void addObserver(const ObserverPtr & objectWatcher);
		//! \param objectWatcher Obserwator DM do wyrejestrowania
		virtual void removeObserver(const ObserverPtr & objectWatcher);

		virtual void getObjects(ConstVariantsList & objects) const;
		//! \param objects [out] Obiekty pasuj¹ce do zapytania
		//! \param type Typ obniektów o które pytamy
		//! \param exact Czy typ musi byæ zgodny czy moga to byæ tez typy pochodne
		virtual void getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const;

		//! \param objects [out] Obiekty zarz¹dzane przez DM
		virtual void getObjects(VariantsCollection& objects) const;

		virtual const bool isManaged(const VariantConstPtr & object) const;

		virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const;

		virtual IDataManagerReader::TransactionPtr transaction() const;

	public:
		// IDataManager API

		virtual void addData(const VariantPtr & data);

		virtual void removeData(const VariantConstPtr & data);

		//! Ta metoda notyfikuje o zmianie stanu DM!!
		//! \param Obiekt który zostanie zmieniony jeœli zarz¹dza nim DataManager, operacja nie musi byæ wykonana od razu
		//! mo¿e trafiæ do kolejki zadañ i byæ wykonana póŸniej - powinno siê obserwowaæ obiekt i wykonaæ specyficzne
		//! operacje po uzyskaniu notyfikacji o zmianie
		virtual void updateData(const VariantConstPtr & data, const VariantConstPtr & newData);

		virtual const bool tryAddData(const VariantPtr & data);

		virtual const bool tryRemoveData(const VariantConstPtr & data);

		virtual const bool tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData);

		virtual IDataManager::TransactionPtr transaction();

	protected:

		const bool rawHasObject(const utils::TypeInfo & type, bool exact) const;

		void rawAddData(const VariantPtr & data);

		void rawRemoveData(const VariantConstPtr & data);

		void rawUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData);

		const bool rawIsManaged(const VariantConstPtr & object) const;

		void rawGetObjects(ConstVariantsList & objects) const;

		void rawGetObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const;

		void rawGetObjects(VariantsCollection& objects) const;

		void updateObservers(const ChangeList & changes);

		static void requestedTypes(const utils::TypeInfo & type, bool exact, utils::TypeInfoSet & types);
	};
}

#endif	//	HEADER_GUARD___MEMORYDATAMANAGER_H__
