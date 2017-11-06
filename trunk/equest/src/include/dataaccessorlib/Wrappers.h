/********************************************************************
created:  2015/08/04	17:09:29
filename: Wrappers.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__WRAPPERS_H__
#define __HEADER_GUARD_DATAACCESSOR__WRAPPERS_H__

#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/WrapperHelpers.h>
#include <dataaccessorlib/Traits.h>
#include <utils/Debug.h>

namespace dataaccessor
{
	//! ------------------------------- Generowane argumenty ---------------------------------------------
	//! \tparam ValuesContainer Typ kontenera z danymi
	//! \tparam ArgumentsGenerator Typ generuj�cy argumenty
	template<typename ValuesContainer, typename ArgumentsGenerator,
		typename ValuesExtractor = ContainerValueExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValuesExtractor>().value(std::declval<ValuesContainer>(), 0)),
		decltype(std::declval<ArgumentsGenerator>().argument(0)) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci�gaj�cy pr�bki z danych
		//! \return Lekki interfejs do danych
		static inline Ptr lightWrap(const ValuesContainer & data, ArgumentsGenerator && ag,
			ValuesExtractor && ve = ValuesExtractor())
	{
		return Ptr(new ContainerDiscreteGeneratedAccessor<typename utils::remove_toplevel<ValuesContainer>::type,
			typename utils::remove_toplevel<ArgumentsGenerator>::type,
			typename utils::remove_toplevel<ValuesExtractor>::type>(data, utils::ContainerSizeExtractor::size(data),
				std::forward<ArgumentsGenerator>(ag), std::forward<ValuesExtractor>(ve)));
	}

	//! ---------------------------- Generowane argumenty + dane ------------------------------------------
	//! \tparam ValuesContainer Typ kontenera z danymi
	//! \tparam ArgumentsGenerator Typ generuj�cy argumenty
	template<typename Value, typename ArgumentsGenerator, typename ValuesExtractor = ContainerValueExtractor,
		typename ContainerExtractor = utils::ValueCarrierDefaultExtractor<typename std::remove_reference<Value>::type>,
		typename Container = typename utils::remove_toplevel<decltype(std::declval<ContainerExtractor>().extract(std::declval<Value>()))>::type,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValuesExtractor>().value(std::declval<Container>(), 0)),
		decltype(std::declval<ArgumentsGenerator>().argument(0)) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci�gaj�cy pr�bki z danych
		//! \return Lekki interfejs do danych
		static inline Ptr wrap(Value && value, ArgumentsGenerator && ag,
			ValuesExtractor && ve = ValuesExtractor(), ContainerExtractor && ce = ContainerExtractor())
	{
		utils::ValueCarrier<Container> cvc(std::forward<Value>(value), std::forward<ContainerExtractor>(ce));
		const auto & c = cvc.ref();
		return Ptr(new ContainerDiscreteGeneratedData<Container, typename utils::remove_toplevel<ArgumentsGenerator>::type,
			typename utils::remove_toplevel<ValuesExtractor>::type>(std::move(cvc),
				utils::ContainerSizeExtractor::size(c), std::forward<ArgumentsGenerator>(ag), std::forward<ValuesExtractor>(ve)));
	}

	//! ---------------------- W�asny ekstraktor ca�ych pr�bek -------------
	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci�gaj�cy pr�bki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci�gaj�cy pr�bki z danych
		//! \return Lekki interfejs do danych
		static inline Ptr lightWrap(const Container & data, SampleExtractor && se = SampleExtractor())
	{
		return Ptr(new ContainerDiscreteAccessor<Container, typename utils::remove_toplevel<SampleExtractor>::type>(data, utils::ContainerSizeExtractor::size(data), std::forward<SampleExtractor>(se)));
	}

	//! ---------------------- W�asne ekstraktory warto�ci i argument�w
	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci�gaj�cy warto�ci z kontenera
	//! \tparam ArgumentExtractor Typ wyci�gaj�cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))) >>
		//! \param data Dane do opakowania
		//! \param ve Obiekt wyci�gaj�cy warto�ci z danych
		//! \param ae Obiekt wyci�gaj�cy argumenty z danych
		//! \return Lekki interfejs do danych
		static inline Ptr lightWrap(const Container & data, ValueExtractor && ve,
			ArgumentExtractor && ae, void * = 0)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return lightWrap<Container, CSE>(data, CSE(std::forward<ValueExtractor>(ve),
			std::forward<ArgumentExtractor>(ae)));
	}

	//! ---------------------- W�asny ekstraktor ca�ych pr�bek -------------
	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci�gaj�cy pr�bki z kontenera
	template<class Value, typename SampleExtractor = DefaultContainerSampleExtractor,
		typename ContainerExtractor = utils::ValueCarrierDefaultExtractor<typename std::remove_reference<Value>::type>,
		typename Container = typename utils::remove_toplevel<decltype(std::declval<ContainerExtractor>().extract(std::declval<Value>()))>::type,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci�gaj�cy pr�bki z danych
		//! \return Interfejs do danych wraz z danymi
		static inline Ptr wrap(Value && value, SampleExtractor && se = SampleExtractor(), ContainerExtractor && ce = ContainerExtractor())
	{
		utils::ValueCarrier<Container> cvc(std::forward<Value>(value), std::forward<ContainerExtractor>(ce));
		const auto & c = cvc.ref();
		return Ptr(new ContainerDiscreteData<Container, typename utils::remove_toplevel<SampleExtractor>::type>(std::move(cvc), utils::ContainerSizeExtractor::size(c), std::forward<SampleExtractor>(se)));
	}

	//! ---------------------- W�asne ekstraktory warto�ci i argument�w
	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci�gaj�cy warto�ci z kontenera
	//! \tparam ArgumentExtractor Typ wyci�gaj�cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))) >>
		//! \param data Dane do opakowania
		//! \param ve Obiekt wyci�gaj�cy warto�ci z danych
		//! \param ae Obiekt wyci�gaj�cy argumenty z danych
		//! \return Interfejs do danych wraz z danymi
		static inline Ptr wrap(Container && data, ValueExtractor && ve,
			ArgumentExtractor && ae)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(std::forward<Container>(data),
			CSE(std::forward<ValueExtractor>(ve), std::forward<ArgumentExtractor>(ae)));
	}

	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci�gaj�cy warto�ci z kontenera
	//! \tparam ArgumentExtractor Typ wyci�gaj�cy argumenty z kontenera
	template<class Value, typename ValueExtractor, typename ArgumentExtractor,
		typename ContainerExtractor = utils::ValueCarrierDefaultExtractor<typename std::remove_reference<Value>::type>,
		typename Container = typename utils::remove_toplevel<decltype(std::declval<ContainerExtractor>().extract(std::declval<Value>()))>::type,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))) >>
		//! \param data Dane do opakowania
		//! \param ve Obiekt wyci�gaj�cy warto�ci z danych
		//! \param ae Obiekt wyci�gaj�cy argumenty z danych
		//! \return Interfejs do danych wraz z danymi
		static inline Ptr wrap(Value && value, ValueExtractor && ve,
			ArgumentExtractor && ae, ContainerExtractor && ce = ContainerExtractor(),
			void * = 0)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(utils::ValueCarrier<Container>(std::forward<Value>(value), std::forward<ContainerExtractor>(ce)),
			CSE(std::forward<ValueExtractor>(ve), std::forward<ArgumentExtractor>(ae)));
	}

	//TODO
	//! -------------------------------- Niezale�ne warto�ci i pr�bki ---------------------



	//! -------------------------------- Rozk�ad wektora na sk�adowe
	struct Vector
	{
		//! Dyskretny
		template<typename AccessorType,
			typename RAT = typename utils::remove_toplevel<AccessorType>::type,
			ENABLE_IF(is_valid_discrete_accessor_ptr<RAT>::value),
			typename Ptr = DiscreteAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<typename AccessorPointerDesc<RAT>::value_type>(), 0)), typename AccessorPointerDesc<RAT>::argument_type >>
			static inline Ptr wrap(AccessorType && accessor, const std::size_t idx)
		{
			return Ptr(new SafeDiscreteAccessorAdapter<typename AccessorPointerDesc<RAT>::value_type, typename AccessorPointerDesc<RAT>::argument_type, utils::ArrayElementExtractor>(std::forward<AccessorType>(accessor), utils::ArrayElementExtractor(idx)));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = DiscreteAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<ValueType>(), 0)), ArgumentType >>
			static inline Ptr lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor, const std::size_t idx)
		{
			return Ptr(new DiscreteAccessorAdapter<ValueType, ArgumentType, utils::ArrayElementExtractor>(accessor, utils::ArrayElementExtractor(idx)));
		}


		//! Ci�g�y
		template<typename AccessorType,
			typename RAT = typename utils::remove_toplevel<AccessorType>::type,
			ENABLE_IF(is_valid_function_accessor_ptr<RAT>::value),
			typename Ptr = FunctionAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<typename AccessorPointerDesc<RAT>::value_type>(), std::declval<typename AccessorPointerDesc<RAT>::argument_type>())), typename AccessorPointerDesc<RAT>::argument_type >>
			static inline Ptr wrap(AccessorType && accessor, const std::size_t idx)
		{
			return Ptr(new SafeFunctionAccessorAdapter<typename AccessorPointerDesc<RAT>::value_type, typename AccessorPointerDesc<RAT>::argument_type, utils::ArrayElementExtractor>(std::forward<AccessorType>(accessor), utils::ArrayElementExtractor(idx)));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = FunctionAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType >>
			static inline Ptr lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & accessor, const std::size_t idx)
		{
			return Ptr(new FunctionAccessorAdapter<ValueType, ArgumentType, utils::ArrayElementExtractor>(accessor, utils::ArrayElementExtractor(idx)));
		}

	};

	template<std::size_t Idx>
	struct StaticVector
	{
		//! Dyskretny
		template<typename AccessorType,
			typename RAT = typename utils::remove_toplevel<AccessorType>::type,
			ENABLE_IF(is_valid_discrete_accessor_ptr<RAT>::value),
			typename Ptr = DiscreteAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<RAT>::value_type>(), 0)), typename AccessorPointerDesc<RAT>::argument_type >>
			static inline Ptr wrap(AccessorType && accessor)
		{
			return Ptr(new SafeDiscreteAccessorAdapter<typename AccessorPointerDesc<RAT>::value_type, typename AccessorPointerDesc<RAT>::argument_type, utils::StaticArrayElementExtractor<Idx>>(std::forward<AccessorType>(accessor)));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = DiscreteAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<ValueType>(), 0)), ArgumentType >>
			static inline Ptr lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return Ptr(new DiscreteAccessorAdapter<ValueType, ArgumentType, utils::StaticArrayElementExtractor<Idx>>(accessor));
		}

		//! Ci�g�y

		template<typename AccessorType,
			typename RAT = typename utils::remove_toplevel<AccessorType>::type,
			ENABLE_IF(is_valid_function_accessor_ptr<RAT>::value),
			typename Ptr = FunctionAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<RAT>::value_type>(), std::declval<typename AccessorPointerDesc<RAT>::argument_type>())), typename AccessorPointerDesc<RAT>::argument_type >>
			static inline Ptr wrap(AccessorType && accessor)
		{
			return Ptr(new SafeFunctionAccessorAdapter<typename AccessorPointerDesc<RAT>::value_type, typename AccessorPointerDesc<RAT>::argument_type, utils::StaticArrayElementExtractor<Idx>>(std::forward<AccessorType>(accessor)));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = FunctionAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType >>
			static inline Ptr lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & accessor)
		{
			return Ptr(new FunctionAccessorAdapter<ValueType, ArgumentType, utils::StaticArrayElementExtractor<Idx>>(accessor));
		}
	};

	//! -------------------------------- Funkcje ------------------------------------------
	//! \tparam T Typ warto�ci
	//! \tparam U Typ argumentu
	template<class T, class U>
	//! \param func Funkcja jednoargumentowa zwracaj�ca warto�ci
	//! \return Interfejs dost�pu do danych
	static inline FunctionAccessorPtr<T, U> lightWrap(T(*const func)(U),
		const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new FunctionAccessor<T(U)>(func, even, odd, monotony));
	}

	//! \tparam Func Typ funkcji
	template<typename Func>
	//! \param func Funkcja jednoargumentowa zwracaj�ca warto�ci
	//! \return Interfejs dost�pu do danych
	static inline typename FuncRet<Func>::type wrap(Func && func, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FuncRet<Func>::type(new FunctionAccessor<typename FuncRet<Func>::R(typename FuncRet<Func>::Arg)>(std::forward<Func>(func), even, odd, monotony));
	}

	//! \tparam T Typ warto�ci
	//! \tparam U Typ argumentu
	//! \tparam Object Typ obiektu
	template<class T, class U, class Object>
	//! \param obj Obiekt dostarczaj�cy danych
	//! \param func Metoda obiektu za pomoca kt�rej pobieramy dane
	//! \return Interfejs dost�pu do danych wrac z danymi
	static inline FunctionAccessorPtr<T, U> lightWrap(const Object & obj,
		T(Object::*const func)(U) const, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new FunctionAccessor<T(Object::*)(U)const>(obj, func, even, odd, monotony));
	}

	//! \tparam T Typ warto�ci
	//! \tparam U Typ argumentu
	//! \tparam Object Typ obiektu
	template<class T, class U, class Object, typename V>
	//! \param obj Obiekt dostarczaj�cy danych
	//! \param func Metoda obiektu za pomoca kt�rej pobieramy dane
	//! \return Interfejs dost�pu do danych wraz z danymi
	static inline FunctionAccessorPtr<T, U> wrap(V && value,
		T(Object::*const func)(U) const, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new DataFunctionAccessor<T, U, Object>(std::forward<V>(value), func, even, odd, monotony));
	}

	template<typename ValueType, typename ArgumentType,
		typename Ptr = DiscreteAccessorPtr < ValueType, ArgumentType >>
		static inline Ptr lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const std::size_t size, const std::size_t start = 0)
	{
		return Ptr(new DiscreteSubAccessorAdapter<ValueType, ArgumentType>(accessor, size, start));
	}

	template<typename ValueType, typename ArgumentType,
		typename Ptr = FunctionAccessorPtr < ValueType, ArgumentType >>
		static inline Ptr lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & end, const ArgumentType & start = ArgumentType())
	{
		return Ptr(new FunctionSubAccessorAdapter<ValueType, ArgumentType>(accessor, end, start));
	}

	template<typename AccessorType, ENABLE_IF(std::is_base_of<IDiscreteAccessorT<typename AccessorType::value_type, typename AccessorType::argument_type>, AccessorType>::value),
		typename Ptr = DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type >>
		static inline Ptr wrap(const utils::shared_ptr<AccessorType> & accessor,
			const std::size_t size, const std::size_t start = 0)
	{
		return Ptr(new SafeDiscreteSubAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type>(accessor, size, start));
	}

	template<typename AccessorType, ENABLE_IF(std::is_base_of<IFunctionAccessorT<typename AccessorType::value_type, typename AccessorType::argument_type>, AccessorType>::value),
		typename Ptr = FunctionAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type >>
		static inline Ptr wrap(const utils::shared_ptr<AccessorType> & accessor,
			const typename AccessorType::argument_type & end, const typename AccessorType::argument_type & start = typename AccessorType::argument_type())
	{
		return Ptr(new SafeFunctionSubAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type>(accessor, end, start));
	}
}

#endif	// __HEADER_GUARD_DATAACCESSOR__WRAPPERS_H__
