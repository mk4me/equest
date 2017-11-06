#include "CoreLibPCH.h"
#include <corelib/VariantsCollection.h>

using namespace core;

VariantsCollection::VariantsCollection(const utils::TypeInfo & typeInfo, bool exact)
	: typeInfo(typeInfo), exact(exact)
{

}

VariantsCollection::VariantsCollection(const VariantsCollection & owc)
	: data(owc.data), typeInfo(owc.typeInfo), exact(owc.exact)
{

}

VariantsCollection::~VariantsCollection()
{

}
const utils::TypeInfo & VariantsCollection::getTypeInfo() const
{
	return typeInfo;
}

//! \return Czy dane musza być dokłdanie tego samego typu dla którego utworzono kolekcję czy mogą też być pochodne od niego
const bool VariantsCollection::exactTypes() const
{
	return exact;
}

//! \param exact Czy dane musza być dokładnie tego samego typu dla którego utworzono kolekcję
//! \param clear Czy czyścić dane niezgodne z typem kolekcji - pochodne mu
void VariantsCollection::setExactTypes(bool exact)
{
	this->exact = exact;

	if(exact == true){
		removeDerivedTypes();
	}
}

VariantsCollection::iterator VariantsCollection::begin()
{
	return data.begin();
}

VariantsCollection::iterator VariantsCollection::end()
{
	return data.end();
}

VariantsCollection::const_iterator VariantsCollection::begin() const
{
	return data.begin();
}

VariantsCollection::const_iterator VariantsCollection::end() const
{
	return data.end();
}

VariantsCollection::reverse_iterator VariantsCollection::rbegin()
{
	return data.rbegin();
}

VariantsCollection::reverse_iterator VariantsCollection::rend()
{
	return data.rend();
}

VariantsCollection::const_reverse_iterator VariantsCollection::crbegin() const
{
	return data.crbegin();
}

VariantsCollection::const_reverse_iterator VariantsCollection::crend() const
{
	return data.crend();
}

const bool VariantsCollection::empty() const
{
	return data.empty();
}

const VariantsCollection::size_type VariantsCollection::size() const
{
	return data.size();
}

void VariantsCollection::push_front(const VariantConstPtr & obj)
{
	insert(data.begin(), obj);
}

void VariantsCollection::pop_front()
{
	data.pop_front();
}

void VariantsCollection::push_back(const VariantConstPtr & obj)
{
	insert(data.end(), obj);
}

void VariantsCollection::pop_back()
{
	data.pop_back();
}

VariantsCollection::reference VariantsCollection::front()
{
	return data.front();
}

VariantsCollection::const_reference VariantsCollection::front() const
{
	return data.front();
}

VariantsCollection::reference VariantsCollection::back()
{
	return data.back();
}

VariantsCollection::const_reference VariantsCollection::back() const
{
	return data.back();
}

VariantsCollection::iterator VariantsCollection::insert(iterator position, const VariantConstPtr& val)
{
	// sprawdzenie poprawności typu
	if (exact == true){
		if(val->data()->getTypeInfo() != getTypeInfo()) {
			// bad_cast nie pobiera parametrów
			//throw std::bad_cast("Type of object not equal to type of collection");
			throw std::bad_cast();
		}
	}
	else if (val->data()->isSupported(typeInfo) == false){
		//throw std::bad_cast("Type of object not supported by collection");
		throw std::bad_cast();
	}

	return data.insert(position, val);
}

VariantsCollection::iterator VariantsCollection::erase(iterator position)
{
	return data.erase(position);
}

VariantsCollection::iterator VariantsCollection::erase(iterator first, iterator last)
{
	return data.erase(first, last);
}

void VariantsCollection::swap(VariantsCollection & owc)
{
	data.swap(owc.data);
	std::swap(typeInfo, owc.typeInfo);
}

void VariantsCollection::clear()
{
	data.clear();
}

void VariantsCollection::remove(const VariantConstPtr& val)
{
	data.remove(val);
}

void VariantsCollection::unique()
{
	data.unique();
}

void VariantsCollection::sort()
{
	data.sort();
}

void VariantsCollection::reverse()
{
	data.reverse();
}

void VariantsCollection::removeDerivedTypes()
{
	//iteruj po kolekcji i usun te typy które nie są dokładnie typu kolekcji
	auto cIT = begin();
	while( cIT != end() ) {
		if((*cIT)->data()->getTypeInfo() != typeInfo){
			cIT = data.erase(cIT);
		}else{
			++cIT;
		}
	}
}
