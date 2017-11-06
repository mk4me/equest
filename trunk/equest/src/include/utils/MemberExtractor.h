/********************************************************************
created:  2016/01/29
filename: MemberExtractor.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__MEMBEREXTRACTOR_H__
#define __HEADER_GUARD_UTILS__MEMBEREXTRACTOR_H__

#include <utils/Macros.h>
#include <utils/pointer_traits.h>
#include <boost/preprocessor.hpp>

namespace utils
{
	template<bool M>
	struct ExtractorBase
	{
		static const bool is_member_extractor = M;
	};

	//! TODO
	//template<typename E>
	//struct is_valid_member_extractor;

	//TODO non portable for Linux (GCC) probably - fix with Boost Processor library

#define GLUE(x, y) x y

	//! Makro z nazw¹ extractora danego pola
#define MEMBER_EXTRACTOR_NAME_1(mN0) MemberExtractor_##mN0
#define MEMBER_EXTRACTOR_NAME_2(mN0, mN1) MEMBER_EXTRACTOR_NAME_1(mN0) ## _ ## mN1
#define MEMBER_EXTRACTOR_NAME_3(mN0, mN1, mN2) MEMBER_EXTRACTOR_NAME_2(mN0, mN1) ## _ ## mN2
#define MEMBER_EXTRACTOR_NAME_4(mN0, mN1, mN2, mN3) MEMBER_EXTRACTOR_NAME_3(mN0, mN1, mN2) ## _ ## mN3
#define MEMBER_EXTRACTOR_NAME_5(mN0, mN1, mN2, mN3, mN4) MEMBER_EXTRACTOR_NAME_4(mN0, mN1, mN2, mN3) ## _ ## mN4
#define MEMBER_EXTRACTOR_NAME_6(mN0, mN1, mN2, mN3, mN4, mN5) MEMBER_EXTRACTOR_NAME_5(mN0, mN1, mN2, mN3, mN4) ## _ ## mN5

#define _MEMBER_EXTRACTOR_NAME_N3(N, ...) MEMBER_EXTRACTOR_NAME_ ## N
#define _MEMBER_EXTRACTOR_NAME_N2(N) _MEMBER_EXTRACTOR_NAME_N3(N)
#define MEMBER_EXTRACTOR_NAME_N(...) GLUE(_MEMBER_EXTRACTOR_NAME_N2(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)), (__VA_ARGS__))

	//! Makro z nazw¹ extractora danego pola
#define MEMBER_EXTRACTOR_BODY_0(value) value
#define MEMBER_EXTRACTOR_BODY_1(value, mN0) MEMBER_EXTRACTOR_NAME_N(mN0)::extract(value)
#define MEMBER_EXTRACTOR_BODY_2(value, mN0, mN1) MEMBER_EXTRACTOR_BODY_1(MEMBER_EXTRACTOR_BODY_1(value, mN0), mN1)
#define MEMBER_EXTRACTOR_BODY_3(value, mN0, mN1, mN2) MEMBER_EXTRACTOR_BODY_1(MEMBER_EXTRACTOR_BODY_2(value, mN0, mN1), mN2)
#define MEMBER_EXTRACTOR_BODY_4(value, mN0, mN1, mN2, mN3) MEMBER_EXTRACTOR_BODY_1(MEMBER_EXTRACTOR_BODY_3(value, mN0, mN1, mN2), mN3)
#define MEMBER_EXTRACTOR_BODY_5(value, mN0, mN1, mN2, mN3, mN4) MEMBER_EXTRACTOR_BODY_1(MEMBER_EXTRACTOR_BODY_4(value, mN0, mN1, mN2, mN3), mN4)
#define MEMBER_EXTRACTOR_BODY_6(value, mN0, mN1, mN2, mN3, mN4, mN5) MEMBER_EXTRACTOR_BODY_1(MEMBER_EXTRACTOR_BODY_5(value, mN0, mN1, mN2, mN3, mN4), mN5)

#define _MEMBER_EXTRACTOR_BODY_N3(N) MEMBER_EXTRACTOR_BODY_ ## N
#define _MEMBER_EXTRACTOR_BODY_N2(N) _MEMBER_EXTRACTOR_BODY_N3(N)
#define MEMBER_EXTRACTOR_BODY_N(value, ...) GLUE(_MEMBER_EXTRACTOR_BODY_N2(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)), (value, __VA_ARGS__))

	//! Makro definuj¹ce dany extractor generyczny
#define MEMBER_EXTRACTOR(memberName) \
struct MEMBER_EXTRACTOR_NAME_N(memberName) : public utils::ExtractorBase<true>{\
/* Wersja dla obiektów */ \
/* \tparam T Typ z którego wypakowujemy membera */ \
/*	\param value Wartoœæ z której wyci¹gamy membera */ \
/*	\param dummy Na potrzeby innych klas */ \
template<typename T> inline static auto extract(const T & value, ...) -> decltype((value.memberName)) { return value.memberName; }\
/* Wersja dla wskaŸników */ \
/* \tparam T Typ z którego wypakowujemy membera */ \
/*	\param value Wartoœæ z której wyci¹gamy membera */ \
/*	\param dummy Na potrzeby innych klas */ \
template<typename T, ENABLE_IF(utils::is_general_pointer<T>::value)> inline static auto extract(const T & value, ...) -> decltype((value->memberName)) { return value->memberName; }\
};

#define NESTED_MEMBER_EXTRACTOR(...) \
struct MEMBER_EXTRACTOR_NAME_N(__VA_ARGS__) : public utils::ExtractorBase<true>{\
/* Wersja dla obiektów */ \
/* \tparam T Typ z którego wypakowujemy membera */ \
/*	\param value Wartoœæ z której wyci¹gamy membera */ \
/*	\param dummy Na potrzeby innych klas */ \
template<typename T> inline static auto extract(const T & value, ...) -> decltype((MEMBER_EXTRACTOR_BODY_N(value, __VA_ARGS__))) { return MEMBER_EXTRACTOR_BODY_N(value, __VA_ARGS__); }\
};

	//! Struktura pomocnicza realizuj¹ca przeŸroczystoœæ wartoœci
	struct TransparentExtractor : public ExtractorBase<false>
	{
		//! \tparam T Typ wartoœci
		template<typename T>
		//! \param value Wartoœæ
		//! \param dummy Na potrzeby innych klas
		//! \return Wartoœæ
		static inline const T & extract(const T & value, ...) { return value; };
	};

	struct PointerExtractor : public ExtractorBase<false>
	{
		//! \tparam T Typ wartoœci
		template<typename T>
		//! \param value Wartoœæ
		//! \param dummy Na potrzeby innych klas
		//! \return Wartoœæ
		static inline auto extract(const T & value, ...) -> decltype(*value) { return *value; };
	};
}

#endif  // __HEADER_GUARD_UTILS__MEMBEREXTRACTOR_H__