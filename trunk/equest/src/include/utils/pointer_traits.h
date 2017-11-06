/********************************************************************
	created:  2016/01/20
	filename: pointer_traits.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__POINTER_TRAITS_H__
#define __HEADER_GUARD_UTILS__POINTER_TRAITS_H__

#include <type_traits>
#include <utils/Utils.h>
#include <utils/SmartPtr.h>

namespace utils
{
	namespace impl {
		//! \tparam T Sprawdzany typ pod k¹tem operatora ->
		template <typename T>
		//! Klasa pomocnicza przy wykrywaniu klas o charakterze smart pointerów
		struct supports_pointer_operator
		{
			//! \tparam U Badany typ
			template<typename U>
			//! \return Typ wy³uskiwany przez operator ->
			static auto test(U* p) -> decltype(p->operator->()); // checks function existence
			//! \tparam U Badany typ
			template<typename U>
			//! \return Fa³sz
			static auto test(...)->std::false_type;
			//! Informacja czy typ ma charakter smart pointera
			using type = typename std::is_pointer<decltype(test<T>(0))>::type; // checks return type is some kind of floating point
		};
	}
	//! \tparam T Sprawdzany typ pod k¹tem operatora ->
	template <typename T>
	//! Klasa z informacj¹ o podobieñstwie do smart pointera
	struct supports_pointer_operator : public impl::supports_pointer_operator<T>::type {};

	//! \tparam T Typ badany pod k¹tem bycia podobnym do smart poitnerów ró¿nej maœci
	template<typename T>
	//! Struktura nios¹ca informacjê czy badany tym ma znamiona smart pointera
	struct is_like_smart_pointer : public std::integral_constant<bool, std::is_class<T>::value && supports_pointer_operator<T>::value> {};

	//! \tparam T Typ badany pod k¹tem bycia podobnym do smart poitnerów ró¿nej maœci
	template<typename T>
	//! Struktura nios¹ca informacjê czy badany tym ma znamiona smart pointera
	struct is_general_pointer : public std::integral_constant<bool, std::is_pointer<T>::value || is_like_smart_pointer<T>::value> {};

	namespace impl
	{
		//! \tparam T Badany typ
		//! \tparam Specjalizacja dla zwyk³ych wksaŸników
		template<typename T, ENABLE_IF(is_general_pointer<T>::value)>
		//! Klasa pomocnicza przy wyci¹ganiu typów wskazywanych przez wskaxniki  i inteligêtne wskaŸniki
		struct pointed_type
		{
			//! Typ wskazywany przez wskaŸnik
			using type = typename std::remove_pointer<typename std::conditional<std::is_pointer<T>::value, T, decltype(std::declval<const T>().operator->())>::type>::type;
		};

		//! \tparam T Badany typ
		//! \tparam Specjalizacja dla zwyk³ych wksaŸników
		template<typename T, ENABLE_IF(is_general_pointer<T>::value)>
		//! Klasa pomocnicza przy wyci¹ganiu typów wskazywanych przez wskaxniki  i inteligêtne wskaŸniki
		struct under_pointer_type
		{
			//! Typ wskazywany przez wskaŸnik
			using type = typename std::conditional<std::is_pointer<T>::value, typename std::remove_pointer<T>::type, decltype(std::declval<const T>().operator->())>::type;
		};
	}

	//! \tparam T Badany typ wskaŸnika
	template<typename T>
	//! Klasa pomocnicza przy wyci¹ganiu ca³ych typów wksazywanych przez wskaŸniki
	struct pointed_type
	{
		//! Typ wskazywany przez wskaŸnik lub wrapowany pod wskaŸnikiem
		using type = typename impl::pointed_type<T>::type;
	};

	//! \tparam T Badany typ wskaŸnika
	template<typename T>
	//! Klasa pomocnicza przy wyci¹ganiu ca³ych typów wksazywanych przez wskaŸniki
	struct under_pointer_type
	{
		//! Typ wskazywany przez wskaŸnik lub wrapowany pod wskaŸnikiem
		using type = typename impl::under_pointer_type<T>::type;
	};

	//! \tparam T Badany typ wskaŸnika
	template<typename T>
	//! Klasa pomocnicza przy wyci¹ganiu "czystych" typów wksazywanych przez wskaŸniki
	struct clean_pointed_type
	{
		//! Typ wskazywany przez wskaŸnik lub wrapowany pod wskaŸnikiem
		using type = typename remove_toplevel<typename impl::pointed_type<T>::type>::type;
	};

	namespace impl
	{
		//! \tparam T Typ wskaŸnika
		//! \tparam dummy
		template<typename T, bool = false>
		//! Specjalizacja dla typów nie maj¹cych charakteru wskaŸnika
		struct pointer_levels
		{
			static const std::size_t levels = 0;
		};

		//! \tparam T Typ tablicy		
		template<typename T>
		//! Specjalizacja dla typów o charakterze tablicy
		struct pointer_levels<T, true>
		{
			using _type = typename pointed_type<T>::type;
			static const std::size_t levels = 1 + pointer_levels<_type, is_general_pointer<_type>::value>::levels;
		};
	}

	//! \tparam Typ badany pod k¹tem bycia tablic¹
	template<typename T>
	//! Wersja specjalizowana dla tablic - wyznacza faktyczny rozmiar tablicy
	struct pointer_levels : public impl::pointer_levels<T, is_general_pointer<T>::value> {};

	namespace impl
	{
		template<typename T, std::size_t Idx>
		struct pointer_level_type
		{
			using type = typename pointer_level_type<typename pointed_type<T>::type, Idx - 1>::type;
		};

		template<typename T>
		struct pointer_level_type<T, 0>
		{
			using type = T;
		};
	}

	//! \tparam T Typ tablicy
	//! \tparam Dim Numer wymiaru tablicy dla któego chcemy pobraæ typ
	//! \tparam dummy Warunek czy typ faktycznie ma charakter tablicy
	template<typename T, std::size_t Level, ENABLE_IF(is_general_pointer<T>::value)>
	struct pointer_level_type
	{
		static_assert(Level > 0 && Level <= pointer_levels<T>::levels, "Index zagnie¿d¿enia musi byæ w przedziale <1 - iloœæ wymiarów tablicy>");

		using type = typename impl::pointer_level_type<typename pointed_type<T>::type, Level - 1>::type;
		using clean_type = typename remove_toplevel<type>::type;

	};

	//! \tparam T Typ tablicy dla której wyci¹gamy typ ostatniego wymiaru		
	template<typename T>
	//! Klasa pomocnicza przy wycaganiu typu ostatniego wymiaru
	struct pointer_final_level_type : public pointer_level_type<T, pointer_levels<T>::levels> {};	
}

#endif  // __HEADER_GUARD_UTILS__POINTER_TRAITS_H__