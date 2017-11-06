#include "CorePCH.h"
#include "RegisteredDataTypesManager.h"
#include "ApplicationCommon.h"
#include <utils/Debug.h>
#include <loglib/Exceptions.h>

namespace core {

	VariantPtr RegisteredDataTypesManager::createWrapper(const utils::TypeInfo & typeInfo) const
{
	auto it = typesHierarchy.find(typeInfo);
	if(it != typesHierarchy.end() && it->second.prototype != nullptr) {
		return Variant::create(it->second.prototype->create());
	}

	return VariantPtr();
}

	void RegisteredDataTypesManager::registeredTypes(utils::TypeInfoSet & registeredTypes) const
{
	for(auto it = typesHierarchy.begin(); it != typesHierarchy.end(); ++it){
		if(it->second.prototype != nullptr){
			registeredTypes.insert(it->first);
		}
	}
}

	const bool RegisteredDataTypesManager::isRegistered(const utils::TypeInfo & typeInfo) const
{
	auto it = typesHierarchy.find(typeInfo);
	if(it == typesHierarchy.end() || it->second.prototype == nullptr) {
		return false;
	}

	return true;
}

	const bool RegisteredDataTypesManager::isDerrived(const utils::TypeInfo & type, const utils::TypeInfo & derrived) const
	{
		bool ret = false;

		auto it = typesHierarchy.find(type);

		if (it != typesHierarchy.end()){
			ret = it->second.derrivedTypes.find(derrived) != it->second.derrivedTypes.end();
		}

		return ret;
	}

	const bool RegisteredDataTypesManager::isBase(const utils::TypeInfo & type, const utils::TypeInfo & base) const
	{
		bool ret = false;

		auto it = typesHierarchy.find(type);

		if (it != typesHierarchy.end()){
			ret = it->second.baseTypes.find(base) != it->second.baseTypes.end();
		}

		return ret;
	}

/*const bool RegisteredDataTypesManager::isTypeCompatible(const utils::TypeInfo & base, const utils::TypeInfo & derrived) const
{
	auto baseIT = typesHierarchy.find(base);
	auto derrivedIT = typesHierarchy.find(derrived);

	if(baseIT == typesHierarchy.end() || derrivedIT == typesHierarchy.end() || baseIT->second.prototype == nullptr || derrivedIT->second.prototype == nullptr){
		return false;
	}

	return (baseIT == derrivedIT) || (derrivedIT->second.baseTypes.find(base) != derrivedIT->second.baseTypes.end());
}*/

	utils::TypeInfoSet RegisteredDataTypesManager::baseTypes(const utils::TypeInfo & type) const
{
	auto it = typesHierarchy.find(type);
	if(it != typesHierarchy.end() && it->second.prototype != nullptr) {
		//baseTypes.insert(it->second.baseTypes.begin(), it->second.baseTypes.end());
		return it->second.baseTypes;
	}

	return utils::TypeInfoSet();
}

	utils::TypeInfoSet RegisteredDataTypesManager::derrivedTypes(const utils::TypeInfo & type) const
{
	auto it = typesHierarchy.find(type);
	if(it != typesHierarchy.end() && it->second.prototype != nullptr) {
		return it->second.derrivedTypes;
	}

	return utils::TypeInfoSet();
}

	void RegisteredDataTypesManager::registerObjectWrapperPrototype(const utils::ObjectWrapperPtr & owp)
{
	UTILS_ASSERT((owp->getRawPtr() == nullptr), "Obiekt powinien byc pusty jako prototyp");
	const utils::TypeInfo & typeInfo = owp->getTypeInfo();
	auto it = typesHierarchy.find(typeInfo);
	if(it == typesHierarchy.end()){
		TypeHierarchy th;
		th.prototype = owp->create();
		it = typesHierarchy.insert(TypesHierarchy::value_type(typeInfo, th)).first;
	}else if(it->second.prototype != nullptr) {
		throw loglib::runtime_error("Type already registered");
	}else{
		it->second.prototype = owp->create();
	}

	//pobieramy typy bazowe
	utils::TypeInfoList baseTypes;
	it->second.prototype->getSupportedTypes(baseTypes);
	baseTypes.remove(typeInfo);
	//inicjujemy typy bazowe
	it->second.baseTypes.insert(baseTypes.begin(), baseTypes.end());
	//aktualizujemy typy bazowe
	for(auto it = baseTypes.begin(); it != baseTypes.end(); ++it){
		auto thIT = typesHierarchy.find(*it);
		if(thIT == typesHierarchy.end()){
			//rejestrujemy typ pochodny przed typem bazowym - nieco niebezpieczne bo nie wiadomo czy typ bazowy będzie potem zarejestrowany
			TypeHierarchy thBase;
			thBase.derrivedTypes.insert(typeInfo);
			typesHierarchy.insert(TypesHierarchy::value_type(*it, thBase));
			CORE_LOG_WARNING("Registering derrived type before base type. Derrived type name: " << typeInfo.name() << ", base type name: " << (*it).name());
		}else{
			thIT->second.derrivedTypes.insert(typeInfo);
		}
	}
}

}
