/********************************************************************
	created:  2016/01/21
	filename: Traits.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__TRAITS_H__
#define __HEADER_GUARD_DATAACCESSOR__TRAITS_H__

#include <dataaccessorlib/Accessors.h>
#include <utils/pointer_traits.h>

namespace dataaccessor
{
	//! \tparam T Typ z którego próbujemy wnioskowaæ wskaŸnik
	template<typename T>
	//! Typ dyskretnego wskaŸnika na bazie akcesora
	using ExtractedDiscreteConstPtr = DiscreteAccessorConstPtr<typename T::value_type, typename T::argument_type>;

	//! \tparam T Typ z którego próbujemy wnioskowaæ wskaŸnik
	template<typename T>
	//! Typ funkcyjnego wskaŸnika na bazie akcesora
	using ExtractedFunctionConstPtr = FunctionAccessorConstPtr<typename T::value_type, typename T::argument_type>;

	namespace impl
	{
		//! \tparam T Typ badany pod k¹tem opisu typów wartoœci i argumentów
		template <typename T>
		//! Klasa pomocnicza przy detekcji kompletnego opisu akcesora (typy wartoœci i argumentów)
		struct has_data_description
		{
			//! \tparam U Typ badany pod k¹tem opisu argumentów
			template<typename U>
			//! \return Prawda jeœli uda siê dopasowaæ
			static auto test_argument_type(typename U::argument_type* p)->std::true_type;
			//! \tparam U Typ badany pod k¹tem opisu argumentów
			template<typename U>
			//! \return Fa³sz jeœli uda siê dopasowaæ
			static auto test_argument_type(...)->std::false_type;

			//! \tparam U Typ badany pod k¹tem opisu wartoœci
			template<typename U>
			//! \return Prawda jeœli uda siê dopasowaæ
			static auto test_value_type(typename U::value_type* p)->std::true_type;
			//! \tparam U Typ badany pod k¹tem opisu wartoœci
			template<typename U>
			//! \return Fa³sz jeœli uda siê dopasowaæ
			static auto test_value_type(...)->std::false_type;

			//! Typ opisuj¹cy dostêpnoœæ informacji o typach wartoœci i argumentów
			using type = std::integral_constant<bool, std::is_same<decltype(test_argument_type<T>(0)), std::true_type>::value &&
				std::is_same<decltype(test_value_type<T>(0)), std::true_type>::value>;
		};
	}

	//! \tparam T Typ badany pod k¹tem opisu typów wartoœci i argumentów
	template<typename T>
	//! Klasa z informacj¹ o kompletnym opisie akcesora
	struct has_data_description : public impl::has_data_description<T>::type {};	

	//! \tparam T Typ z którego wnioskujemy dyskretny akcesor
	template<typename T>
	//! Typ dyskretnego akcesora na bazie innego typu
	using ExtractedDiscreteAccessor = IDiscreteAccessorT<typename T::value_type, typename T::argument_type>;

	//! \tparam T Typ z którego wnioskujemy ci¹g³y akcesor
	template<typename T>
	//! Typ ci¹g³ego akcesora na bazie innego typu
	using ExtractedFunctionAccessor = IFunctionAccessorT<typename T::value_type, typename T::argument_type>;

	namespace impl
	{
		//! \tparam T Badany typ pod k¹tem dyskretnego akcesora
		//! \tparam dummy
		template <typename T, bool>
		//! Domyœlna implementacja - fa³sz
		struct is_valid_discrete_accessor : public std::false_type {};

		//! \tparam T Badany typ pod k¹tem dyskretnego akcesora
		template <typename T>
		//! Implementacja kiedy typ ma opis odpowiadaj¹cy akcesorowi - prawda kiedy spe³nione s¹ pozosta³e warunki
		struct is_valid_discrete_accessor<T, true> : public std::integral_constant<bool, std::is_base_of<ExtractedDiscreteAccessor<T>, T>::value && !std::is_base_of<ExtractedFunctionAccessor<T>, T>::value> {};
	}

	//! \tparam T Badany typ pod k¹tem dyskretnego akcesora
	template<typename T>
	//! Klasa z informacj¹ czy badany tym ma charakter dyskretnego akcesora
	struct is_valid_discrete_accessor : public impl::is_valid_discrete_accessor<T, has_data_description<T>::value>::type {};

	namespace impl
	{
		//! \tparam T Badany typ pod k¹tem funkcyjnego akcesora
		//! \tparam dummy
		template <typename T, bool>
		//! Domyœlna implementacja - fa³sz
		struct is_valid_function_accessor : public std::false_type {};

		//! \tparam T Badany typ pod k¹tem funkcyjnego akcesora
		template <typename T>
		//! Implementacja kiedy typ ma opis odpowiadaj¹cy akcesorowi - prawda kiedy spe³nione s¹ pozosta³e warunki
		struct is_valid_function_accessor<T, true> : public std::integral_constant<bool, !std::is_base_of<ExtractedDiscreteAccessor<T>, T>::value && std::is_base_of<ExtractedFunctionAccessor<T>, T>::value> {};
	}

	//! \tparam T Badany typ pod k¹tem funkcyjnego akcesora
	template<typename T>
	//! Klasa z informacj¹ czy badany tym ma charakter funkcyjnego akcesora
	struct is_valid_function_accessor : public impl::is_valid_function_accessor<T, has_data_description<T>::value>::type {};	

	//! \tparam T Typ sprawdzany pod k¹tem poprawnoœci akcesora
	template<typename T>
	//! Typ z informacj¹ o poprawnoœci akcesora (jeden z interfejsów)
	struct is_valid_accessor : public utils::xor_check<is_valid_function_accessor<T>::value, is_valid_discrete_accessor<T>::value>::type {};

	//! \tparam T Typ implementuj¹cy jeden z interfejsów akcesorów (dyskretny/funkcja)
	//! \tparam dummy Weryfikacja ¿e mamy do czynienia z poprawnym akcesorem
	template<typename T, ENABLE_IF(is_valid_accessor<T>::value)>
	struct accessor_interface
	{
		using type = typename std::conditional<is_valid_discrete_accessor<T>::value, typename ExtractedDiscreteAccessor<T>::type, typename ExtractedFunctionAccessor<T>::type>::type;
	};

	namespace impl
	{
		//! \tparam T Typ weryfikowany pod k¹tem poprawnoœci wskaŸnika do akcesora
		//! \tparam dummy
		template<typename T, bool>
		//! Czeœciowa specjalizacja - fa³sz
		struct is_valid_discrete_accessor_ptr : public std::false_type {};

		//! \tparam T Typ weryfikowany pod k¹tem poprawnoœci wskaŸnika do akcesora
		template<typename T>
		//! Czêœciowa specjalizacja - weryfikacja warunków	
		struct is_valid_discrete_accessor_ptr<T, true> : public std::is_convertible<T, ExtractedDiscreteConstPtr<typename utils::clean_pointed_type<T>::type>>::type {};
	}

	//! \tparam T Typ weryfikowany pod k¹tem poprawnoœci wskaŸnika do akcesora
	template<typename T>
	//! Czêœciowa specjalizacja - weryfikacja warunków
	struct is_valid_discrete_accessor_ptr : public impl::is_valid_discrete_accessor_ptr<T, utils::is_like_smart_pointer<T>::value> {};

	namespace impl
	{
		//! \tparam T Typ weryfikowany pod k¹tem poprawnoœci wskaŸnika do akcesora
		//! \tparam dummy
		template<typename T, bool>
		//! Czeœciowa specjalizacja - fa³sz
		struct is_valid_function_accessor_ptr : public std::false_type {};

		//! \tparam T Typ weryfikowany pod k¹tem poprawnoœci wskaŸnika do akcesora
		template<typename T>
		//! Czêœciowa specjalizacja - weryfikacja warunków	
		struct is_valid_function_accessor_ptr<T, true> : public std::is_convertible<T, ExtractedFunctionConstPtr<typename utils::clean_pointed_type<T>::type>>::type {};
	}

	//! \tparam T Typ weryfikowany pod k¹tem poprawnoœci wskaŸnika do akcesora
	template<typename T>
	//! Czêœciowa specjalizacja - weryfikacja warunków
	struct is_valid_function_accessor_ptr : public impl::is_valid_function_accessor_ptr<T, utils::is_like_smart_pointer<T>::value> {};

	//! \tparam T Typ weryfikowany pod k¹tem poprawnoœci wskaŸnika do akcesora
	//! \tparam dummy
	template<typename T>
	//! Czeœciowa specjalizacja - fa³sz
	struct is_valid_accessor_ptr : public utils::xor_check<is_valid_function_accessor_ptr<T>::value, is_valid_discrete_accessor_ptr<T>::value>::type {};	
	//! \tparam T Typ wskaxnika do akcesora
	//! \tparam dummy Weryfikacja wks¹Ÿnika
	template<typename T, ENABLE_IF(is_valid_accessor_ptr<T>::value ||
		(std::is_pointer<T>::value && is_valid_accessor<typename utils::clean_pointed_type<T>::type>::value))>
	//! Klasa pomocnicza do wyci¹gania informacji o akcesorze ze wskaŸników
	struct AccessorPointerDesc
	{
		using accessor = typename utils::clean_pointed_type<T>::type;
		using value_type = typename accessor::value_type;
		using argument_type = typename accessor::argument_type;
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__TRAITS_H__