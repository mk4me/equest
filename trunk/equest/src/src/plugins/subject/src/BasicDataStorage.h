/********************************************************************
	created:  2013/02/07
	created:  7:2:2013   11:24
	filename: BasicDataStorage.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___BASICDATASTORAGE_H__
#define HEADER_GUARD___BASICDATASTORAGE_H__

#include <map>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>
#include <mutex>

class BasicDataStorage
{
private:

	//! Typ mapy obiektów.
	typedef std::map< utils::TypeInfo, core::ConstVariantsSet > ObjectsByTypes;
	//! Typ synchronizacji
	typedef std::recursive_mutex SyncPolicy;
	//! Typ lokalnej synchronizacji
	typedef std::lock_guard<SyncPolicy> ScopedLock;

private:

	//! Obiekty pochodz¹ce z parserów.
	ObjectsByTypes objectsByTypes;
	//! Obiekt synchronizuj¹cy
	mutable SyncPolicy sync;

public:
	//! Domyœlny konstruktor
	BasicDataStorage();
	//! Destruktor
	~BasicDataStorage();
	//! \param objects [out] Placeholder na przechowywane i pobierane obiekty
	void getObjects(core::ConstVariantsList & objects) const;
	//! \param objects [out] Obiekty pasuj¹ce do zapytania
	//! \param type Typ obniektów o które pytamy
	//! \param exact Czy typ musi byæ zgodny czy moga to byæ tez typy pochodne
	void getObjects(core::ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const;
	//! \param objects [out] Obiekty zarz¹dzane przez DM
	void getObjects(core::VariantsCollection& objects) const;

	const bool isManaged(const core::VariantConstPtr & object) const;

	const bool hasObject(const utils::TypeInfo & type, bool exact) const;

	void addData(const core::VariantConstPtr & data);

	void removeData(const core::VariantConstPtr & data);

	const bool tryAddData(const core::VariantConstPtr & data);

	const bool tryRemoveData(const core::VariantConstPtr & data);

private:

	static void requestedTypes(const utils::TypeInfo & type, bool exact, utils::TypeInfoSet & types);
	std::pair<ObjectsByTypes::iterator, core::ConstVariantsSet::iterator> find(const core::VariantConstPtr & data);
};

#endif	//	HEADER_GUARD___BASICDATASTORAGE_H__
