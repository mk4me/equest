/********************************************************************
	created:  2014/08/06	18:30:28
	filename: ObjectWrapperT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/

#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__
	#error This is implementation specific private file. This header should newer be included explicitely. Instead You should include ObjectWrapper.h
#endif

#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPERT_H__
#define __HEADER_GUARD_UTILS__OBJECTWRAPPERT_H__

namespace utils
{
	//! Pomocniczy typ bazowy, zarządzający obiektem za pomocą parametru
	//! PtrPolicy. Tego typu nigdy nie używa się wprost.
	template <typename T>
	class __ObjectWrapperT : public ObjectWrapper
	{
	public:
		//! Typ wrapowanych danych
		using Type = T;
		//! Pełny typ aktualnego wrappera
		using ImplType = __ObjectWrapperT<Type>;

	private:
		//! Wrappowany obiekt.
		typename ObjectWrapperTraits<Type>::Ptr wrapped_;	

	protected:

		//! Konstruktor
		//! \param data Dane		
		__ObjectWrapperT(typename ObjectWrapperTraits<Type>::Ptr data)
			: wrapped_(data)
		{
		}
		
		//! Domyslny konstruktor
		__ObjectWrapperT()
		{
			ObjectWrapperTraits<Type>::PtrPolicy::initPtr(wrapped_);
		}

		//! Domyslny konstruktor
		__ObjectWrapperT(__ObjectWrapperT && Other) : wrapped_(std::move(Other.wrapped_))
		{

		}

	private:

		//! \param dest ObjectWrapper do którego chcemy kopiować dane
		//! \return Wskaźnik na ObjectWrapper mojego typu lub rzuca wyjątkiem
		static ImplType * __getMe(ObjectWrapper & dest)
		{
			auto cloned = dynamic_cast<ImplType*>(&dest);

			if (cloned == nullptr){
				throw std::runtime_error("Clone destination wrapper type mismatch");
			}

			return cloned;
		}

		//! \param cloned Wskaźnik na ObjectWrapper mojego typu
		//! \param co Typ klonowania
		void __cloneImpl(ImplType * cloned, const ObjectWrapper::CloneOp co) const
		{
			if (co == ObjectWrapper::ShallowClone){
				cloned->wrapped_ = wrapped_;
			}
			else{
				typename ObjectWrapperTraits<Type>::Ptr newPtr(ObjectWrapperTraits<Type>::ClonePolicy::clone(&*wrapped_));
				cloned->wrapped_ = newPtr;
			}
		}

		virtual void __setData(const void * object) override
		{
			ObjectWrapperTraits<Type>::PtrPolicy::setPtr(wrapped_, *(static_cast<const typename ObjectWrapperTraits<Type>::Ptr *>(object)));
		}

		virtual bool __tryGetData(void * object, const TypeInfo & ptrType) override
		{
			return ObjectWrapperTraits<Type>::tryGetData(object, ptrType, wrapped_);
		}

		virtual bool __tryGetData(void * object, const TypeInfo & ptrType) const override
		{
			return ObjectWrapperTraits<Type>::tryGetConstData(object, ptrType, wrapped_);
		}

		virtual bool __tryGetBaseData(void * object, const TypeInfo & ptrType) override
		{
			return ObjectWrapperTraits<Type>::tryGetBaseData(object, ptrType, wrapped_);
		}

		virtual bool __tryGetBaseData(void * object, const TypeInfo & ptrType) const override
		{
			return ObjectWrapperTraits<Type>::tryGetConstBaseData(object, ptrType, wrapped_);
		}

		virtual bool __isEqual(const ObjectWrapper & obj) const override
		{
			typename ObjectWrapperTraits<Type>::ConstPtr w;
			bool res = obj.tryGet(w);

			return (res == true) && (wrapped_ == w);
		}

		virtual void __clone(ObjectWrapper & dest, const ObjectWrapper::CloneOp co) const  override
		{
			auto me = __getMe(dest);
			__cloneImpl(me, co);
		}

		virtual const void* __getRawPtr() const override
		{
			return ObjectWrapperTraits<Type>::PtrPolicy::getConstRawPtr(wrapped_);
		}

		virtual void* __getRawPtr() override
		{
			return ObjectWrapperTraits<Type>::PtrPolicy::getRawPtr(wrapped_);
		}

		virtual void __swap(ObjectWrapper & ow) override
		{
			auto me = __getMe(ow);
			ObjectWrapperTraits<Type>::PtrPolicy::swapPtr(wrapped_, me->wrapped_);
		}

		virtual void __reset() override
		{
			ObjectWrapperTraits<Type>::PtrPolicy::setPtr(wrapped_, static_cast<T*>(nullptr));
		}

	public:
		//! Destruktor wirtualny
		virtual ~__ObjectWrapperT()
		{
		} 

		virtual int getReferenceCount() const override
		{
			return ObjectWrapperTraits<Type>::PtrPolicy::referenceCount(wrapped_);
		}

		virtual bool isPtrSupported(const TypeInfo & ptrInfo) const override
		{
			return ObjectWrapperTraits<Type>::ptrTypeSupported(ptrInfo);
		}

		virtual ObjectWrapper::TypeInfoPair getPtrTypeInfo() const override
		{
			return ObjectWrapperTraits<Type>::ptrTypeInfo();
		}

		//! \return Nazwa typu.
		virtual std::string getClassName() const override
		{
			return ObjectWrapperT<Type>::className();
		}

		//! \return Informacje o typie.
		virtual TypeInfo getTypeInfo() const override
		{
			return ObjectWrapperTraits<Type>::typeInfo();
		}

		virtual void getSupportedTypes(Types& supported) const override
		{
			ObjectWrapperTraits<Type>::supportedTypes(supported);
		}

		virtual bool isSupported(const TypeInfo& type) const override
		{
			return ObjectWrapperTraits<Type>::typeSupported(type);
		}

		virtual ObjectWrapperPtr create() const  override 
		{
			return ObjectWrapper::create<T>();
		}	
	};

	//! Deklaracja typu. Trzeba go specjalizować za pomocą makr. Ta wersja będzie
	//! rzucać statyczną asercję.
	template <typename T>
	class ObjectWrapperT : public ObjectWrapper
	{
		static_assert(sizeof(T) == 0, "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");	
	};
}

#define __DEFINE_WRAPPER_CLASS(typeT)\
template <> class ObjectWrapperT<typeT> : public __ObjectWrapperT<typeT>{\
private:\
	ObjectWrapperT() {} \
	ObjectWrapperT(ObjectWrapperTraits<typeT>::Ptr data) : __ObjectWrapperT<typeT>(data) {}\
public:\
	virtual ~ObjectWrapperT() {}\
	static std::string className(){\
		return #typeT;\
	}\
	static ObjectWrapperPtr create(ObjectWrapperTraits<typeT>::Ptr data = ObjectWrapperTraits<typeT>::Ptr()){\
		return ObjectWrapperPtr(data ? new ObjectWrapperT : new ObjectWrapperT(data));\
	}\
	static ObjectWrapperPtr wrap(ObjectWrapperTraits<typeT>::Ptr data){\
		return ObjectWrapperPtr(new ObjectWrapperT(data)); \
	}\
};

#endif	// __HEADER_GUARD_UTILS__OBJECTWRAPPERT_H__
