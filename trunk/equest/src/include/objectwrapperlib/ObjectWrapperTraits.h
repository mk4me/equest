/********************************************************************
	created:  2014/04/29	14:33:22
	filename: ObjectWrapperTraits.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPERTRAITS_H__
#define __HEADER_GUARD_UTILS__OBJECTWRAPPERTRAITS_H__

#include <utils/Debug.h>
#include <utils/TypeInfo.h>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

	template <class T>
	struct ObjectWrapperTraits
	{
		static const bool isDefinitionVisible = false;

		struct PtrPolicy
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
		};

		struct Ptr
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
		};

		struct ConstPtr
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
		};

		struct ClonePolicy
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
		};

		template<class PtrIn>
		static bool tryGetData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return false;
		}

		template<class PtrIn>
		static bool tryGetConstData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{		
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return false;
		}

		/*static const ObjectWrapper::TypeInfoPair ptrTypeInfo()
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return ObjectWrapper::TypeInfoPair();
		}*/

		static bool ptrTypeSupported(const TypeInfo & ptrInfo)
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return false;
		}
		
		static bool typeSupported(const TypeInfo & type)
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return false;
		}
		
		/*static void supportedTypes(ObjectWrapper::Types & types)
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
		}*/
		
		static TypeInfo typeInfo()
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return TypeInfo();
		}

		template<class PtrIn>
		static bool tryGetBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return false;
		}

		template<class PtrIn>
		static bool tryGetConstBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{
			static_assert(sizeof(T) == 0, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
			return false;
		}
	};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#define __DEFINE_WRAPPER_META_HELPER \
	template<class PtrIn>\
	static bool tryGetData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped){\
		auto p = ptrTypeInfo();\
		if (ptrType == p.first) { *(static_cast<Ptr*>(object)) = wrapped; }\
		else if (ptrType == p.second) {	*(static_cast<ConstPtr*>(object)) = wrapped; }\
		else {\
			UTILS_ASSERT((false), "Unexpected behavior unpacking wrapper data");\
			return false;\
		}\
		return true;\
	}\
	template<class PtrIn>\
	static bool tryGetConstData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped){\
		if (ptrType == ptrTypeInfo().second){ *(static_cast<ConstPtr*>(object)) = wrapped; }\
		else{\
			UTILS_ASSERT((false), "Unexpected behavior unpacking const wrapper data");\
			return false;\
		}\
		return true;\
	}


////////////////////////////////////////////////////////////////////////////////
// Makra dla definiowaia wrapperów
////////////////////////////////////////////////////////////////////////////////

#define __DEFINE_WRAPPER_META(typeT, ptrPolicyT, clonePolicyT)\
	template <> struct ObjectWrapperTraits<typeT>\
{\
	static const bool isDefinitionVisible = true;\
	using PtrPolicy = ptrPolicyT;\
	using Ptr = PtrPolicy::Ptr<typeT>::Type;\
	using ConstPtr = PtrPolicy::Ptr<const typeT>::Type;\
	using ClonePolicy = clonePolicyT;\
	static ObjectWrapper::TypeInfoPair ptrTypeInfo(){\
		return ObjectWrapper::TypeInfoPair(typeid(Ptr), typeid(ConstPtr));\
	}\
	static bool ptrTypeSupported(const TypeInfo & ptrInfo){\
		const auto p = ptrTypeInfo();\
		return ptrInfo == p.first || ptrInfo == p.second;\
	}\
	static bool typeSupported(const TypeInfo & type){\
		return type == typeInfo();\
	}\
	static void supportedTypes(ObjectWrapper::Types & types){\
		types.push_back(typeInfo());\
	}\
	static TypeInfo typeInfo(){\
		return typeid(typeT);\
	}\
	__DEFINE_WRAPPER_META_HELPER \
	template<class PtrIn>\
	static bool tryGetBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped){\
		UTILS_ASSERT((false), "Nie mozna pobierac typu bazowego dla tego ObjectWrappera - on jest baza");\
		return false;\
	}\
	template<class PtrIn>\
	static bool tryGetConstBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped){\
		UTILS_ASSERT((false), "Nie mozna pobierac typu bazowego dla tego ObjectWrappera - on jest baza");\
		return false;\
	}\
};

#define __DEFINE_WRAPPER_DERIVED_META(typeT, baseTypeT)\
	template <> struct ObjectWrapperTraits<typeT>\
{\
	static_assert((ObjectWrapperTraits<baseTypeT>::isDefinitionVisible == true), "Missing wrapper definition for type baseTypeT");\
	static_assert((std::is_base_of<baseTypeT, typeT>::value), "Type typeT must inherit from baseTypeT");\
	static const bool isDefinitionVisible = true;\
	using PtrPolicy = ObjectWrapperTraits<baseTypeT>::PtrPolicy;\
	using Ptr = PtrPolicy::Ptr<typeT>::Type;\
	using ConstPtr = PtrPolicy::Ptr<const typeT>::Type;\
	using ClonePolicy = ObjectWrapperTraits<baseTypeT>::ClonePolicy;\
	static ObjectWrapper::TypeInfoPair ptrTypeInfo(){\
		return ObjectWrapper::TypeInfoPair(typeid(Ptr), typeid(ConstPtr));\
	}\
	static bool ptrTypeSupported(const TypeInfo & ptrInfo){\
		const auto p = ptrTypeInfo();\
		return ptrInfo == p.first || ptrInfo == p.second || ObjectWrapperTraits<baseTypeT>::ptrTypeSupported(ptrInfo);\
	}\
	static void supportedTypes(ObjectWrapper::Types & types){\
		types.push_back(typeInfo()); ObjectWrapperTraits<baseTypeT>::supportedTypes(types);\
	}\
	static bool typeSupported(const TypeInfo & type){\
		return type == typeInfo() || ObjectWrapperTraits<baseTypeT>::typeSupported(type);\
	}\
	static const TypeInfo typeInfo(){\
		return typeid(typeT);\
	}\
	__DEFINE_WRAPPER_META_HELPER \
	template<class PtrIn>\
	static bool tryGetBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped){\
		if (ObjectWrapperTraits<baseTypeT>::ptrTypeInfo().first == ptrType ||\
				ObjectWrapperTraits<typeT>::ptrTypeInfo().second == ptrType){\
			return ObjectWrapperTraits<baseTypeT>::tryGetData(object, ptrType, wrapped);\
						}\
		return ObjectWrapperTraits<baseTypeT>::tryGetBaseData(object, ptrType, wrapped);\
	}\
	template<class PtrIn>\
	static bool tryGetConstBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped){\
		if (ObjectWrapperTraits<baseTypeT>::ptrTypeInfo().second == ptrType){\
			return ObjectWrapperTraits<baseTypeT>::tryGetConstData(object, ptrType, wrapped);\
						}\
		return ObjectWrapperTraits<baseTypeT>::tryGetConstBaseData(object, ptrType, wrapped);\
	}\
};

#endif	// __HEADER_GUARD_UTILS__OBJECTWRAPPERTRAITS_H__
