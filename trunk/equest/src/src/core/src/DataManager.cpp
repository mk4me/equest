#include "CorePCH.h"
#include "DataManager.h"
#include "RegisteredDataTypesManager.h"
#
#include "ApplicationCommon.h"
#include <loglib/Exceptions.h>

using namespace core;

class DataManager::Transaction : public IDataManager::ITransaction
{
private:
	DataManager * mdm;	
	utils::shared_ptr<std::lock_guard<std::recursive_mutex>> lock;
	DataManager::ChangeList modyfications;
	bool transactionRollbacked;

public:
	Transaction(DataManager * mdm) : mdm(mdm),
		lock(new std::lock_guard<std::recursive_mutex>(mdm->sync)),
		transactionRollbacked(false)
	{		
	}

	~Transaction()
	{
		if(transactionRollbacked == false){
			if(modyfications.empty() == false){
				mdm->updateObservers(modyfications);
			}			
		}
	}

public:

	virtual const bool isRolledback() const
	{
		return transactionRollbacked;
	}

	virtual void rollback()
	{
		transactionRollbacked = true;

		ConstVariantsSet firstChanges;

		//szybkie cofanie edycji - tylko pierwotna jest przywracana
		for(auto it = modyfications.begin(); it != modyfications.end(); ++it){
			if((*it).modyfication == IDataManagerReader::UPDATE_OBJECT && firstChanges.find((*it).currentValue) == firstChanges.end())
			{
				firstChanges.insert((*it).currentValue);
				mdm->rawUpdateData((*it).currentValue, (*it).previousValue);
			}
		}

		//cofanie pozostałych zmian od końca
		for(auto it = modyfications.rbegin(); it != modyfications.rend(); ++it){
			switch((*it).modyfication){

			case IDataManagerReader::ADD_OBJECT:
				mdm->rawRemoveData((*it).currentValue);
				break;

			case IDataManagerReader::REMOVE_OBJECT:
				mdm->rawAddData(utils::const_pointer_cast<Variant>((*it).previousValue));
				break;
			}
		}

		lock.reset();
	}

	//! \data Dane wchodzące pod kontrolę DM
	virtual void addData(const VariantPtr & data)
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		if(mdm->rawIsManaged(data) == true){
			rollback();
			throw loglib::runtime_error("Memory transaction tried to add data already managed by manager");
		}

		rawAddData(data);
	}

	//! Dane usuwane z DM
	virtual void removeData(const VariantConstPtr & data)
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		if(mdm->rawIsManaged(data) == false){
			rollback();
			throw loglib::runtime_error("Memory transaction tried to remove data not managed by manager");
		}

		rawRemoveData(data);
	}

	//! \param data Aktualizowane dane
	//! \param newData Nowa wartość danych
	virtual void updateData(const VariantConstPtr & data, const VariantConstPtr & newData)
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		if(mdm->rawIsManaged(data) == false){
			rollback();
			throw loglib::runtime_error("Memory transaction tried to edit data not managed by manager");
		}

		try{
			rawUpdateData(data, newData);
		}catch(...){
			rollback();
			throw;
		}
	}

	virtual const bool tryAddData(const VariantPtr & data)
	{
		if(transactionRollbacked == true || mdm->rawIsManaged(data) == true){
			return false;
		}

		try{
			rawAddData(data);
		}catch(...){
			return false;
		}

		return true;
	}

	virtual const bool tryRemoveData(const VariantConstPtr & data)
	{
		if(transactionRollbacked == true || mdm->rawIsManaged(data) == false){
			return false;
		}

		try{
			rawRemoveData(data);
		}catch(...){
			return false;
		}

		return true;
	}

	virtual const bool tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
	{
		if(transactionRollbacked == true || mdm->rawIsManaged(data) == false){
			return false;
		}

		try{
			rawUpdateData(data, newData);
		}catch(...){
			return false;
		}

		return true;
	}

	virtual void getObjects(ConstVariantsList & objects) const
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects);
	}

	virtual void getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects, type, exact);
	}

	virtual void getObjects(VariantsCollection& objects) const
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects);
	}

	virtual const bool isManaged(const VariantConstPtr & object) const
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		return mdm->rawIsManaged(object);
	}

	virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const
	{
		if(transactionRollbacked == true){
			throw loglib::runtime_error("Memory transaction rolled-back");
		}

		return mdm->rawHasObject(type, exact);
	}

private:

	void rawAddData(const VariantPtr & data)
	{
		//dodajemy dane do dm
		mdm->rawAddData(data);
		//aktualizujemy liste zmian
		Change change;
		change.currentValue = data;
		change.modyfication = IDataManagerReader::ADD_OBJECT;
		change.type = data->data()->getTypeInfo();
		modyfications.push_back(change);
	}

	void rawRemoveData(const VariantConstPtr & data)
	{
		//dodajemy dane do dm
		mdm->rawRemoveData(data);
		//aktualizujemy liste zmian
		Change change;
		change.previousValue = data;
		change.modyfication = IDataManagerReader::REMOVE_OBJECT;
		change.type = data->data()->getTypeInfo();
		modyfications.push_back(change);
	}

	void rawUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
	{
		Change change;
		change.previousValue = data->clone();
		//dodajemy dane do dm
		mdm->rawUpdateData(data, newData);
		//aktualizujemy liste zmian
		change.currentValue = data;
		change.modyfication = IDataManagerReader::UPDATE_OBJECT;
		change.type = data->data()->getTypeInfo();
		modyfications.push_back(change);
	}
};

class DataManager::ReaderTransaction : public IDataManagerReader::IOperations
{
public:
	ReaderTransaction(DataManager * mdm) : mdm(mdm)
	{
		mdm->sync.lock();
	}

	~ReaderTransaction()
	{
		mdm->sync.unlock();
	}

public:

	virtual void getObjects(ConstVariantsList & objects) const
	{
		mdm->rawGetObjects(objects);
	}

	virtual void getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
	{
		mdm->rawGetObjects(objects, type, exact);
	}

	virtual void getObjects(VariantsCollection& objects) const
	{
		mdm->rawGetObjects(objects);
	}

	virtual const bool isManaged(const VariantConstPtr & object) const
	{
		return mdm->rawIsManaged(object);
	}

	virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const
	{
		return mdm->rawHasObject(type, exact);
	}


private:
	DataManager * mdm;
};

DataManager::DataManager()
{

}

DataManager::~DataManager()
{
	ObjectsByTypes().swap(objectsByTypes);
}

void DataManager::addObserver(const ObserverPtr & objectWatcher)
{
	ScopedLock lock(sync);
	if(std::find(observers.begin(), observers.end(), objectWatcher) != observers.end()){
		throw loglib::runtime_error("Watcher already registered");
	}

	observers.push_back(objectWatcher);
}

void DataManager::removeObserver(const ObserverPtr & objectWatcher)
{
	ScopedLock lock(sync);
	auto it = std::find(observers.begin(), observers.end(), objectWatcher);
	if(it == observers.end()){
		throw loglib::runtime_error("Watcher not registered");
	}

	observers.erase(it);
}

void DataManager::getObjects(ConstVariantsList & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects);
}

void DataManager::getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects, type, exact);
}

void DataManager::getObjects(VariantsCollection& objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects);
}

const bool DataManager::isManaged(const VariantConstPtr & object) const
{
	ScopedLock lock(sync);
	return rawIsManaged(object);
}

void DataManager::addData(const VariantPtr & data)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == true){
		throw loglib::runtime_error("Object already managed by manager");
	}

	rawAddData(data);
	ChangeList changes;
	Change change;
	change.currentValue = data;
	change.modyfication = IDataManagerReader::ADD_OBJECT;
	change.type = data->data()->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

void DataManager::removeData(const VariantConstPtr & data)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == false){
		throw loglib::runtime_error("Object not managed by manager");
	}

	rawRemoveData(data);

	ChangeList changes;
	Change change;
	change.previousValue = data;
	change.modyfication = IDataManagerReader::REMOVE_OBJECT;
	change.type = data->data()->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

void DataManager::updateData(const VariantConstPtr & data, const VariantConstPtr & newData)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == false){
		throw loglib::runtime_error("Object not managed by manager");
	}

	ChangeList changes;
	Change change;
	change.previousValue = data->clone();

	rawUpdateData(data, newData);

	change.currentValue = data;

	change.modyfication = IDataManagerReader::UPDATE_OBJECT;
	change.type = data->data()->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

const bool DataManager::tryAddData(const VariantPtr & data)
{
	bool ret = true;

	try{
		addData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool DataManager::tryRemoveData(const VariantConstPtr & data)
{
	bool ret = true;

	try{
		removeData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool DataManager::tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
{
	bool ret = true;

	try{
		updateData(data, newData);
	}catch(...){
		ret = false;
	}

	return ret;
}

IDataManager::TransactionPtr DataManager::transaction()
{	
	return IDataManager::TransactionPtr(new Transaction(this));
}

IDataManagerReader::TransactionPtr DataManager::transaction() const
{	
	return IDataManagerReader::TransactionPtr(new ReaderTransaction(const_cast<DataManager*>(this)));
}

void DataManager::rawGetObjects(ConstVariantsList & objects) const
{
	for(auto it = objectsByTypes.begin(); it != objectsByTypes.end(); ++it){
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	}
}

void DataManager::rawGetObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
{
	utils::TypeInfoSet types;
	requestedTypes(type, exact, types);

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end()){
			objects.insert(objects.end(), dataIT->second.begin(), dataIT->second.end());
		}
	}
}

void DataManager::rawGetObjects(VariantsCollection& objects) const
{
	ConstVariantsList locObjects;
	rawGetObjects(locObjects, objects.getTypeInfo(), objects.exactTypes());
	objects.nonCheckSplice(objects.end(), locObjects);
}

void DataManager::rawAddData(const VariantPtr & data)
{
	objectsByTypes[data->data()->getTypeInfo()].insert(data);
}

void DataManager::rawRemoveData(const VariantConstPtr & data)
{
	auto it = objectsByTypes.find(data->data()->getTypeInfo());
	if (it != objectsByTypes.end()){
		it->second.erase(utils::const_pointer_cast<Variant>(data));
		if (it->second.empty() == true){
			objectsByTypes.erase(it);
		}
	}
}

void DataManager::rawUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
{
	//TODO
	//assign value - clone?

	auto d = utils::const_pointer_cast<Variant>(data);

	d->copyData(*newData);
	d->copyMetadata(*newData);
}

const bool DataManager::rawIsManaged(const VariantConstPtr & object) const
{
	auto type = object->data()->getTypeInfo();
	if(getRegisteredDataTypesManager()->isRegistered(type) == false){
		throw loglib::runtime_error(std::string("Type not registered: ") + type.name());
	}

	auto it = objectsByTypes.find(type);
	if(it != objectsByTypes.end() && std::find(it->second.begin(), it->second.end(), object) != it->second.end()){
		return true;
	}

	return false;
}

void DataManager::updateObservers(const ChangeList & changes )
{
	for(auto it = observers.begin(); it != observers.end(); ++it){
		try{
			(*it)->observe(changes);
		}catch(...){
			//TODO
			//rozwinąć obserwatorów aby się jakoś identyfikowali!! ewentualnie robić to przez własną implementację dostarczaną konretnym obiektom
			//(osobne interfejsy readerów dla każdego elemnentu ładowanego do aplikacji - service, source, datasink, itp)
			CORE_LOG_WARNING("Error while updating memory data manager observer");
		}
	}
}

const bool DataManager::hasObject(const utils::TypeInfo & type, bool exact) const
{
	ScopedLock lock(sync);
	return rawHasObject(type, exact);
};


const bool DataManager::rawHasObject(const utils::TypeInfo & type, bool exact) const
{
	bool ret = false;

	utils::TypeInfoSet types;
	requestedTypes(type, exact, types);

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end() && dataIT->second.empty() == false){
			ret = true;
			break;
		}
	}

	return ret;
}

void DataManager::requestedTypes(const utils::TypeInfo & type, bool exact, utils::TypeInfoSet & types)
{
	types.insert(type);

	if(exact == false){
		auto dt = getRegisteredDataTypesManager()->derrivedTypes(type);
		types.insert(dt.begin(), dt.end());
	}
}

