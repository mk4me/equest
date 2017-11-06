/********************************************************************
created:  2016/01/26
filename: container_traits.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__CONTAINER_TRAITS_H__
#define __HEADER_GUARD_UTILS__CONTAINER_TRAITS_H__

#include <utils/array_traits.h>
#include <iterator>

namespace utils
{
	namespace impl
	{
		//! \tparam C Typ kontenera, kt�ry weryfikujemy
		template<typename C>
		//! Struktura pomocnicza przy weryfikacji tablic (operator [])
		struct empty_member_check
		{
			//! Weryfikacja operatora [] w wersji const
			template<typename U> static auto check(const U * u) -> decltype(u->empty());
			//! Fallback
			template<typename U> static auto check(...)->std::false_type;
			//! Informacja o operatorze
			using type = typename std::is_same<bool, decltype(check<C>(0))>::type;
		};

		//! \tparam C Typ kontenera, kt�ry weryfikujemy
		template<typename C>
		//! Struktura pomocnicza przy weryfikacji tablic (operator [])
		struct size_member_check
		{
			//! Weryfikacja operatora [] w wersji const
			template<typename U> static auto check(const U * u) -> decltype(u->size());
			//! Fallback
			template<typename U> static auto check(...)->std::false_type;
			//! Informacja o operatorze
			using type = typename std::is_integral<decltype(check<C>(0))>::type;
		};

		//! \tparam C Typ kontenera, kt�ry weryfikujemy
		template<typename C>
		//! Struktura pomocnicza przy weryfikacji dost�pu do iterator�w
		struct iterator_based_container_check
		{
			//! Weryfikacja operatora [] w wersji const
			template<typename U> static auto check_begin(const U * u) -> decltype(u->begin());
			//! Fallback
			template<typename U> static auto check_begin(...)->std::false_type;
			//! Weryfikacja operatora [] w wersji const
			template<typename U> static auto check_end(const U * u) -> decltype(u->begin());
			//! Fallback
			template<typename U> static auto check_end(...)->std::false_type;
			//! Weryfikacja operatora [] w wersji const
			template<typename U> static auto check_iterator_traits(typename std::iterator_traits<U>::pointer u)->std::true_type;
			//! Fallback
			template<typename U> static auto check_iterator_traits(...)->std::false_type;
			//! Informacja o operatorze
			using type = typename std::integral_constant<bool, std::is_same<decltype(check_iterator_traits<decltype(check_begin<C>(0))>(0)), std::true_type>::value &&
				std::is_same<decltype(check_iterator_traits<decltype(check_end<C>(0))>(0)), std::true_type>::value>;
		};
	}

	//! \tparam C Typ kontenera, kt�ry weryfikujemy
	template<typename C>
	//! Struktura pomocnicza przy weryfikacji tablic (operator [])
	struct empty_member_check : public impl::empty_member_check<C>::type {};

	//! \tparam C Typ kontenera, kt�ry weryfikujemy
	template<typename C>
	//! Struktura pomocnicza przy weryfikacji tablic (operator [])
	struct size_member_check : public impl::size_member_check<C>::type {};

	//! \tparam C Typ kontenera, kt�ry weryfikujemy
	template<typename C>
	//! Struktura pomocnicza przy weryfikacji tablic (operator [])
	struct iterator_based_container_check : public impl::iterator_based_container_check<C>::type {};

	//! \tparam T Typ tablicy
	template <typename T, ENABLE_IF(!is_general_array<T>::value && iterator_based_container_check<T>::value)>
	//! \param array Tablica
	//! \return Wypakowany element
	static inline auto extract(const T & array, const std::size_t idx) -> decltype(*(array.begin()))
	{
		auto it = array.begin();
		std::advance(it, idx);
		return *it;
	}

	//! Wypakowuje z tablicy zadanyc element
	//! \tparam Element Indeks elementu w tablicy	
	template <std::size_t Element>
	using StaticContainerElementExtractor = StaticArrayElementExtractor<Element>;

	//! Klasa pozwalaj�ca wypakowywa� dane z wektor�w
	using ContainerElementExtractor = ElementExtractor;

	//! Zwraca d�ugo�� tablicy
	//! \param fixedArray Tablica
	//! \return D�ugo�� tablicy
	template <class T>
	static inline typename std::enable_if<size_member_check<T>::value, std::size_t>::type size(const T & array)
	{
		return array.size();
	}

	//! Zwraca d�ugo�� tablicy
	//! \param fixedArray Tablica
	//! \return D�ugo�� tablicy
	template <class T>
	static inline typename std::enable_if<!size_member_check<T>::value && iterator_based_container_check<T>::value, std::size_t>::type size(const T & array)
	{
		return std::distance(array.begin(), array.end());
	}

	//! Struktura pomocnicza przy wyci�ganiu rozmiaru tablicy
	struct ContainerSizeExtractor
	{
		//! \tparam T Typ tablicy
		template<typename T>
		//! \param array Tablica kt�rej rozmiar chcemy wyci�gn��
		//! \return Rozmiar tablicy
		static inline std::size_t size(const T & array)
		{
			return utils::size(array);
		}
	};

	//! Zwraca d�ugo�� tablicy
	//! \param fixedArray Tablica
	//! \return D�ugo�� tablicy
	template <class T>
	static inline typename std::enable_if<empty_member_check<T>::value, bool>::type empty(const T & array)
	{
		return array.empty();
	}

	//! Struktura pomocnicza przy sprawdzaniu czy tablica jest pusta
	struct ContainerEmptinessExtractor
	{
		//! \tparam T Typ tablicy
		template<typename T>
		//! \param array Tablica kt�r� sprawdzamy czy jest pusta
		//! \return Czy tablica jest pusta
		static inline std::size_t empty(const T & array)
		{
			return utils::empty(array);
		}
	};
}

#endif  // __HEADER_GUARD_UTILS__CONTAINER_TRAITS_H__
