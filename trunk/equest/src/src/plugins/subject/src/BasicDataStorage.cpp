#include "SubjectPCH.h"
#include "BasicDataStorage.h"
#include <corelib/PluginCommon.h>
#include <corelib/IDataHierarchyManagerReader.h>

BasicDataStorage::BasicDataStorage()
{

}

BasicDataStorage::~BasicDataStorage()
{

}

void BasicDataStorage::getObjects(core::ConstVariantsList & objects) const
{
	ScopedLock lock(sync);
	for(auto it = objectsByTypes.begin(); it != objectsByTypes.end(); ++it){
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	}
}

void BasicDataStorage::getObjects(core::ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
{
	utils::TypeInfoSet types;
	requestedTypes(type, exact, types);

	ScopedLock lock(sync);

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end()){
			objects.insert(objects.end(), dataIT->second.begin(), dataIT->second.end());
		}
	}
}

void BasicDataStorage::getObjects(core::VariantsCollection& objects) const
{
	core::ConstVariantsList locObjects;
	getObjects(locObjects, objects.getTypeInfo(), objects.exactTypes());	
	objects.insert(objects.end(), locObjects.begin(), locObjects.end());
}

const bool BasicDataStorage::isManaged(const core::VariantConstPtr & object) const
{
	auto type = object->data()->getTypeInfo();
	if(plugin::getRegisteredDataTypesManagerReader()->isRegistered(type) == false){
		throw loglib::runtime_error(std::string("Type not registered: ") + type.name());
	}

	ScopedLock lock(sync);

	auto it = objectsByTypes.find(object->data()->getTypeInfo());
	if(it != objectsByTypes.end() && std::find(it->second.begin(), it->second.end(), object) != it->second.end()){
		return true;
	}

	return false;
}

const bool BasicDataStorage::hasObject(const utils::TypeInfo & type, bool exact) const
{
	bool ret = false;

	utils::TypeInfoSet types;
	requestedTypes(type, exact, types);

	ScopedLock lock(sync);

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

void BasicDataStorage::addData(const core::VariantConstPtr & data)
{
	auto type = data->data()->getTypeInfo();
	if (plugin::getRegisteredDataTypesManagerReader()->isRegistered(type) == false){
		throw loglib::runtime_error(std::string("Type not registered: ") + type.name());
	}

	ScopedLock lock(sync);

	auto found = find(data);
	if(found.first != objectsByTypes.end() && found.second != found.first->second.end()){
		throw loglib::runtime_error("Object already managed");
	}

	if(found.first != objectsByTypes.end()){
		found.first->second.insert(data);
	}else{
		objectsByTypes[data->data()->getTypeInfo()].insert(data);
	}
}

void BasicDataStorage::removeData(const core::VariantConstPtr & data)
{
	auto type = data->data()->getTypeInfo();
	if (plugin::getRegisteredDataTypesManagerReader()->isRegistered(type) == false){
		throw loglib::runtime_error(std::string("Type not registered: ") + type.name());
	}

	ScopedLock lock(sync);

	auto found = find(data);
	if(found.first == objectsByTypes.end() || found.second == found.first->second.end()){
		throw loglib::runtime_error("Object not managed");
	}

	found.first->second.erase(found.second);

	if(found.first->second.empty() == true){
		objectsByTypes.erase(found.first);
	}
}

const bool BasicDataStorage::tryAddData(const core::VariantConstPtr & data)
{
	bool ret = true;
	try{
		addData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool BasicDataStorage::tryRemoveData(const core::VariantConstPtr & data)
{
	bool ret = true;
	try{
		removeData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

void BasicDataStorage::requestedTypes( const utils::TypeInfo & type, bool exact, utils::TypeInfoSet & types )
{
	types.insert(type);

	if(exact == false){
		auto t = plugin::getRegisteredDataTypesManagerReader()->derrivedTypes(type);
		types.insert(t.begin(), t.end());
	}
}

std::pair<BasicDataStorage::ObjectsByTypes::iterator, core::ConstVariantsSet::iterator> BasicDataStorage::find(const core::VariantConstPtr & data)
{
	std::pair<BasicDataStorage::ObjectsByTypes::iterator, core::ConstVariantsSet::iterator> ret;

	ret.first = objectsByTypes.find(data->data()->getTypeInfo());
	if(ret.first != objectsByTypes.end()){
		ret.second = ret.first->second.find(data);
	}

	return ret;
}