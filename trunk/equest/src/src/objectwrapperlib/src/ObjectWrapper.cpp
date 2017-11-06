#include <objectwrapperlib/ObjectWrapper.h>

using namespace utils;

ObjectWrapper::get_t ObjectWrapper::get(bool exact)
{
	get_t result = { this, nullptr, exact };
	return result;
}

//! Pobiera obiekt z wrappera. W razie b³êdu rzuca bad_castem.
//! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
//! \return Return Type Resolver
const ObjectWrapper::get_t ObjectWrapper::get(bool exact) const
{
	const get_t result = { nullptr, this, exact };
	return result;
}

ObjectWrapper::ObjectWrapper()
{

}

ObjectWrapper::ObjectWrapper(ObjectWrapper &&)
{

}

bool ObjectWrapper::__tryUnpackData(void * object, const TypeInfo & ptrType)
{
	return __tryGetData(object, ptrType);
}


bool ObjectWrapper::__tryUnpackData(void * object, const TypeInfo & ptrType) const
{
	return __tryGetData(object, ptrType);
}

bool ObjectWrapper::__tryUnpackBaseData(void * object, const TypeInfo & ptrType)
{
	return __tryGetBaseData(object, ptrType);
}


bool ObjectWrapper::__tryUnpackBaseData(void * object, const TypeInfo & ptrType) const
{
	return __tryGetBaseData(object, ptrType);
}

const void* ObjectWrapper::getRawPtr() const
{
	return __getRawPtr();
}

void* ObjectWrapper::getRawPtr()
{
	return __getRawPtr();
}

ObjectWrapper::~ObjectWrapper()
{

}

bool ObjectWrapper::isSupported(const TypeInfo& type) const
{
	return type == getTypeInfo();
}

void ObjectWrapper::clone(ObjectWrapper & dest, const CloneOp co) const
{
	__clone(dest, co);
}

bool ObjectWrapper::tryClone(ObjectWrapper & dest, const CloneOp co) const
{
	bool ret = true;
	try{
		clone(dest, co);
	}
	catch (...){
		ret = false;
	}

	return ret;
}

ObjectWrapperPtr ObjectWrapper::clone(const CloneOp co) const
{	
	auto ret = create();
	clone(*ret, co);
	return ret;
}

void ObjectWrapper::reset()
{
	__reset();
}

void ObjectWrapper::swap(ObjectWrapper & ow)
{
	__swap(ow);	
}

bool ObjectWrapper::isEqual(const ObjectWrapper & obj) const
{
	bool ret = false;
	// najpierw sprawdzamy czy liczniki referencji siê zgadzaj¹
	if (getReferenceCount() == obj.getReferenceCount()) {
		// teraz czy jak conajmniej jeden z nich ró¿ny od zera to czy raw pointers siê zgadzaj¹
		if ((getRawPtr() != nullptr) && (obj.getRawPtr() != nullptr)) {
			ret = (getRawPtr() == obj.getRawPtr());
		}
		//czy ma sens probowaæ ich dalej porównywaæ - czy maj¹ jakieœ typy wspólne
		else {

			if (isSupported(obj.getTypeInfo()) == true) {
				ret = __isEqual(obj);
			}
			else if (obj.isSupported(getTypeInfo()) == true) {
				ret = obj.__isEqual(*this);
			}
		}
	}

	return ret;
}

bool operator==(const ObjectWrapper & a, const ObjectWrapper & b)
{
	return a.isEqual(b);
}

bool operator!=(const ObjectWrapper & a, const ObjectWrapper & b)
{
	return !(a == b);
}