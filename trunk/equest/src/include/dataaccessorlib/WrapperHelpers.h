/********************************************************************
created:  2015/07/27	09:45:53
filename: WrapperHelpers.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__WRAPPERHELPERS_H__
#define __HEADER_GUARD_DATAACCESSOR__WRAPPERHELPERS_H__

#include <dataaccessorlib/SafeAccessorWrapper.h>
#include <dataaccessorlib/FunctionFeature.h>
#include <iterator>
#include <utils/function_traits.h>
#include <utils/container_traits.h>

namespace dataaccessor
{
	//! Taka cz�sciowa specjalizacja dzia�a jak enable_if, daj�c nam potrzebne typy,
	//! lub brak ich definicji - SFINAE i eliminacja niepasuj�cych sygnatur wrpaer�w
	template<typename Func, bool = std::is_one_argument_functor<Func>::value>
	struct FuncRet
	{
		using R = typename std::function_traits<Func>::return_type;
		using Arg = typename std::function_traits<Func>:: template argument<0>::type;
		using type = FunctionAccessorPtr<R, Arg>;
	};

	template<typename Func>
	struct FuncRet<Func, false> {};

	//! Extractor warto�ci kontenera
	template<class ElementExtractor = utils::TransparentExtractor>
	struct GeneralContainerValueExtractor
	{
	public:

		//! \tparam C Typ kontenera
		template<typename C, typename VE = decltype(utils::ContainerElementExtractor::extract(std::declval<C>(), 0))>
		//! \param C Kontener dla kt�ego pobieramy element
		//! \param idx Indeks elementu kt�ry chcemy pobra� z kontenera
		//! \return Element kontenera o podanym indeksie
		static inline auto value(const C & c, const std::size_t idx) -> decltype(ElementExtractor::extract(std::declval<VE>(), idx))
		{
			return ElementExtractor::extract(utils::ContainerElementExtractor::extract(c, idx), idx);
		}
	};

	//! Extractor warto�ci kontenera
	using ContainerValueExtractor = GeneralContainerValueExtractor<>;

	//! Struktura pomocnicza przy wyci�ganiu argumentu z pary
	struct PairArgumentExtractor
	{
		//! \tparam T Pierwszy typ w parze
		//! \tparam U Drugi typ w parze
		template<typename T, typename U>
		//! \param sample Pr�bka
		//! \return argument pr�bki
		static inline const T & value(const std::pair<T, U> & sample)
		{
			return sample.first;
		}
	};

	//! Struktura pomocnicza przy wyci�ganiu warto�ci z pary
	struct PairValueExtractor
	{
		//! \tparam T Pierwszy typ w parze
		//! \tparam U Drugi typ w parze
		template<typename T, typename U>
		//! \param sample Pr�bka
		//! \return Warto�� pr�bki
		static inline const U & value(const std::pair<T, U> & sample)
		{
			return sample.second;
		}
	};

	//! Domy�lny extraktor pr�bek z kontener�w
	struct DefaultContainerSampleExtractor
	{
		//! \tparam C Typ kontenera
		template<typename C>
		//! \param container Kontener z kt�rego pobieramy pr�bk�
		//! \param idx Numer pr�bki kt�r� chcemy pobra�
		//! \return Pr�bka o zadanym indeksie
		static inline auto sample(const C & container, const std::size_t idx) -> decltype(ContainerValueExtractor::value(container, idx))
		{
			return ContainerValueExtractor::value(container, idx);
		}
	};

	//! \tparam SValueExtractor Typ extraktora varto�ci z pr�bek kontenera
	//! \tparam SArgumentExtractor Typ extraktora argument�w z pr�bek kontenera
	//! \tparam CValueExtractor Typ extraktora pr�bek z kontenera
	template<typename SValueExtractor,
		typename SArgumentExtractor,
		typename CValueExtractor = ContainerValueExtractor>
		//! Klasa pomocnicza przy wypakowywaniu pr�bek z kontener�w
		class ContainerSampleExtractor : private CValueExtractor,
		private SValueExtractor, private SArgumentExtractor
	{
	public:

		//! \tparam C Typ kontenera
		template<typename C>
		//! Typ warto�ci w kontenerze - z nich wyci�gamy argumenty i warto�ci
		using CValue = decltype(std::declval<CValueExtractor>().value(std::declval<C>(), 0));

		//! \tparam C Typ kontenera
		template<typename C>
		struct Sample
		{
			//! Musi tak by�, w przeciwnym razie VS ma problemy
			typedef typename IAccessorT<typename std::decay<decltype(std::declval<SValueExtractor>().value(std::declval<CValue<C>>()))>::type,
				typename std::decay<decltype(std::declval<SArgumentExtractor>().value(std::declval<CValue<C>>()))>::type>::sample_type type;
		};

	public:

		ContainerSampleExtractor(const ContainerSampleExtractor & Other)
			: SValueExtractor(Other),
			SArgumentExtractor(Other),
			CValueExtractor(Other)
		{

		}

		ContainerSampleExtractor(ContainerSampleExtractor && Other)
			: SValueExtractor(std::move(Other)),
			SArgumentExtractor(std::move(Other)),
			CValueExtractor(std::move(Other))
		{

		}

		template<typename SVT = SValueExtractor,
			typename SAT = SArgumentExtractor,
			typename CVT = CValueExtractor>
			ContainerSampleExtractor(SVT && svalueExtractor,
				SAT && sargumentExtractor,
				CVT && cvalueExtractor = CValueExtractor())
			: CValueExtractor(std::forward<CVT>(cvalueExtractor)),
			SValueExtractor(std::forward<SAT>(svalueExtractor)),
			SArgumentExtractor(std::forward<SVT>(sargumentExtractor))
		{

		}

		//! Destruktor
		virtual ~ContainerSampleExtractor() {}

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param container Kontener z kt�rego wypakowujemy pr�bki accessora
		//! \param idx Indeks pr�bki kt�r� chemy wypakowa�
		//! \return Pr�bka accessora kt�r� wypakowali�my z kontenera
		inline typename Sample<C>::type sample(const C & container, const std::size_t idx) const
		{
			using RCVal = decltype(CValueExtractor::value(container, idx));
			RCVal cval = CValueExtractor::value(container, idx);
			return typename Sample<C>::type(SArgumentExtractor::value(cval), SValueExtractor::value(cval));
		}
	};

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora pr�bek akcesora	
	template<typename Container,
		typename SampleExtractor = DefaultContainerSampleExtractor>
		//! Implementacja dyskrentego akseora dla kontener�w danych
		class ContainerDiscreteAccessor : public IOptimizedDiscreteAccessorT<
		typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().second)>::type,
		typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().first)>::type>,
		private SampleExtractor
	{
	public:

		using BaseType = IOptimizedDiscreteAccessorT<
			typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().second)>::type,
			typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().first)>::type>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;
		using size_type = typename BaseType::size_type;

	public:

		template<typename ST = SampleExtractor>
		ContainerDiscreteAccessor(const Container & container, const std::size_t size,
			ST && sampleExtractor = ST())
			: SampleExtractor(std::forward<ST>(sampleExtractor)), container(container),
			size_(size), empty_(size == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~ContainerDiscreteAccessor() {}

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{
			return SampleExtractor::sample(container, idx);
		}

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:
		//! Kontener z danymi
		const Container & container;
		//! Rozmiar kontenrea
		const std::size_t size_;
		//! Czy kontener jest pusty
		const bool empty_;
	};

	template<typename Container,
		typename SampleExtractor = DefaultContainerSampleExtractor>
		using ContainerDiscreteData = SafeAccessorWrapper<ContainerDiscreteAccessor<Container, SampleExtractor>, Container>;

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora pr�bek akcesora	
	template<typename Container,
		typename ArgumentsGenerator,
		typename ValueExtractor = ContainerValueExtractor>
		//! Implementacja dyskrentego akseora dla kontener�w danych
		class ContainerDiscreteGeneratedAccessor : public IIndependentDiscreteAccessorT<
		typename std::decay<decltype(std::declval<ValueExtractor>().value(std::declval<Container>(), 0))>::type,
		typename std::decay<decltype(std::declval<ArgumentsGenerator>().argument(0))>::type>,
		private ArgumentsGenerator, private ValueExtractor
	{
	public:

		using BaseType = IIndependentDiscreteAccessorT<
			typename std::decay<decltype(std::declval<ValueExtractor>().value(std::declval<Container>(), 0))>::type,
			typename std::decay<decltype(std::declval<ArgumentsGenerator>().argument(0))>::type>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;
		using size_type = typename BaseType::size_type;

	public:
		template<typename AT = ArgumentsGenerator,
			typename VT = ValueExtractor>
			ContainerDiscreteGeneratedAccessor(const Container & container, const std::size_t size,
				AT && argumentsGenerator = AT(),
				VT && valuesExtractor = VT())
			: ArgumentsGenerator(std::forward<AT>(argumentsGenerator)),
			ValueExtractor(std::forward<VT>(valuesExtractor)),
			container(container), size_(size), empty_(size == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~ContainerDiscreteGeneratedAccessor() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentsGenerator::argument(idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return ValueExtractor::value(container, idx); }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:
		//! Kontener z danymi
		const Container & container;
		//! Rozmiar kontenrea
		const std::size_t size_;
		//! Czy kontener jest pusty
		const bool empty_;
	};

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora pr�bek akcesora	
	template<typename Container,
		typename ArgumentsGenerator,
		typename ValueExtractor = ContainerValueExtractor>
		using ContainerDiscreteGeneratedData = SafeAccessorWrapper<ContainerDiscreteGeneratedAccessor<Container, ArgumentsGenerator, ValueExtractor>, Container>;

	//! \tparam ValuesContainer Typ kontenera z warto�ciami
	//! \tparam ArgumentsContainer Typ kontenera z argumentami
	//! \tparam ValueExtractor Typ obiektu wypakowuj�cego warto�ci z kontenera warto�ci
	//! \tparam ArgumentExtractor Typ obiektu wypakowuj�cego argumenty z kontenera argument�w
	template<typename ValuesContainer, typename ArgumentsContainer,
		typename ValueExtractor, typename ArgumentExtractor>
		//! Implementacja realizuje dost�p do kana�u dyskretnego w oparciu o niezale�ne kontenery wartosci i pr�bek
		class IndependentContainersDiscreteAccessor : public IIndependentDiscreteAccessorT<
		typename std::decay<decltype(std::declval<ValueExtractor>().value(std::declval<ValuesContainer>(), 0))>::type,
		typename std::decay<decltype(std::declval<ArgumentExtractor>().value(std::declval<ArgumentsContainer>(), 0))>::type>,
		private ArgumentExtractor, private ValueExtractor
	{
	public:

		using BaseType = IIndependentDiscreteAccessorT<
			typename std::decay<decltype(std::declval<ValueExtractor>().value(std::declval<ValuesContainer>(), 0))>::type,
			typename std::decay<decltype(std::declval<ArgumentExtractor>().value(std::declval<ArgumentsContainer>(), 0))>::type>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;
		using size_type = typename BaseType::size_type;

	public:

		template<typename VT = ValueExtractor,
			typename AT = ArgumentExtractor>
			IndependentContainersDiscreteAccessor(const ValuesContainer & values,
				const ArgumentsContainer & arguments, const std::size_t vsize,
				const std::size_t asize, VT && valueExtractor = VT(),
				AT && argumentExtractor = AT())
			: ValueExtractor(std::forward<VT>(valueExtractor)),
			ArgumentExtractor(std::forward<AT>(argumentExtractor)),
			values(values), arguments(arguments), size_(std::min(vsize, asize)),
			empty_(size_ == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~IndependentContainersDiscreteAccessor() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentExtractor::value(arguments, idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return ValueExtractor::value(values, idx); }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:
		//! Kontener z warto�ciami
		const ValuesContainer & values;
		//! Kontener z argumentami
		const ArgumentsContainer & arguments;
		//! Rozmiar danych
		const std::size_t size_;
		//! Czy pusty akcesor
		const bool empty_;
	};

	//! Klasa realizuj�ca zadanie wrappera dla kontenera - tag pozwala na dziedziczenie
	//! po tej klasie przy takich samych typach kontener�w warto�ci i argument�w
	//! za spraw� taga mamy 2 niezale�ne typy
	template<typename Container, typename Tag>
	using TaggedContainerCarrier = utils::TypeTagWrapper<utils::ValueCarrier<Container>, Tag>;

	//! Typy tag�w
	struct Tag {
		//! Warto�ci
		class ValueContainer;
		//! Argumenty
		class ArgumentContainer;
	};

	//! \tparam ValuesContainer Typ kontenera z warto�ciami
	//! \tparam ArgumentsContainer Typ kontenera z argumentami
	//! \tparam ValueExtractor Typ obiektu wypakowuj�cego warto�ci z kontenera warto�ci
	//! \tparam ArgumentExtractor Typ obiektu wypakowuj�cego argumenty z kontenera argument�w
	template<typename ValuesContainer, typename ArgumentsContainer,
		typename ValueExtractor, typename ArgumentExtractor>
		//! Implementacja realizuje dost�p do kana�u dyskretnego w oparciu o niezale�ne kontenery wartosci i pr�bek
		class IndependentContainersDiscreteData : private TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>,
		private TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>,
		public IndependentContainersDiscreteAccessor<ValuesContainer, ArgumentsContainer,
		ValueExtractor, ArgumentExtractor>
	{
	public:

		using BaseType = IndependentContainersDiscreteAccessor<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;
		using size_type = typename BaseType::size_type;

	public:

		template<typename VCT = utils::ValueCarrier<ValuesContainer>,
			typename ACT = utils::ValueCarrier<ArgumentsContainer>,
			typename VT = ValueExtractor,
			typename AT = ArgumentExtractor>
			IndependentContainersDiscreteData(VCT && values,
				ACT && arguments, const std::size_t vsize,
				const std::size_t asize, VT && valueExtractor = VT(),
				AT && argumentExtractor = AT())
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(std::forward<VCT>(values)),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(std::forward<ACT>(arguments)),
			IndependentContainersDiscreteAccessor<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::ref(),
				TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::ref(), vsize, asize,
				std::forward<VT>(valueExtractor), std::forward<AT>(argumentExtractor))
		{

		}

		//! Destruktor wirtualny
		virtual ~IndependentContainersDiscreteData() {}
	};

	template<typename ArgumentsGenerator, typename ValueType>
	class ConstDiscreteData : public IIndependentDiscreteAccessorT<typename std::decay<ValueType>::type, typename std::decay<decltype(std::declval<ArgumentsGenerator>().argument(0))>::type>,
		private ArgumentsGenerator
	{
	public:

		using BaseType = IIndependentDiscreteAccessorT<typename std::decay<ValueType>::type, typename std::decay<decltype(std::declval<ArgumentsGenerator>().argument(0))>::type>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;
		using size_type = typename BaseType::size_type;

	public:

		template<typename AT = ArgumentsGenerator,
			typename VT = ValueType>
			ConstDiscreteData(AT && argumentsGenerator, VT && value) :
			ArgumentsGenerator(std::forward<AT>(argumentsGenerator)),
			value_(std::forward<VT>(value))
		{

		}

		//! Destruktor wirtualny
		virtual ~ConstDiscreteData() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentsGenerator::argument(idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return value_; }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return ArgumentsGenerator::size(); }

	private:
		//! Warto��
		const ValueType value_;
	};

	template<typename ValueType, typename ArgumentType>
	class ConstFunctionData : public IGeneratedFunctionAccessorT<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>
	{
	public:

		using BaseType = IGeneratedFunctionAccessorT<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;

	public:

		template<typename VT = ValueType>
		//! \param value Warto�� kana�u
		ConstFunctionData(VT && value) : value_(std::forward<VT>(value))
		{

		}

		//! Destruktor wirtualny
		virtual ~ConstFunctionData() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual value_type value(const argument_type & argument) const override { return value_; }

	private:
		//! Warto��
		const value_type value_;
	};

	//! \tparam
	template<typename>
	//! Forward declaration opisu funkcji ci�g�ej
	class FunctionAccessor;

	template<typename ValueType, typename ArgumentType>
	class FunctionAccessorBase : public IGeneratedFunctionAccessorT<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>
	{
	public:

	public:

		using BaseType = IGeneratedFunctionAccessorT<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;

	public:

		//! \param even
		//! \param odd
		//! \param monotony
		FunctionAccessorBase(const bool even = false, const bool odd = false,
			const MonotonyType monotony = NonMonotonic)
		{
			this->attachFeature(FunctionFeature::feature(true));
			this->attachFeature(IFeaturePtr(new FunctionDescriptionFeature(even, odd, monotony)));
		}

		//! Virtual destructor
		virtual ~FunctionAccessorBase() {}
	};

	//! \tparam Ret Typ warto�ci zwracanej z funckji
	//! \tparam Arg Typ warto�ci argumentu funkcji
	template<typename Ret, typename Arg>
	//! Implementacja wrappera dla funkcji ci�g�ej
	class FunctionAccessor<Ret(Arg)> : public FunctionAccessorBase<Ret, Arg>
	{
	public:

		using BaseType = FunctionAccessorBase<Ret, Arg>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;

	public:
		//! \tparam Func Typ funktora
		template<typename Func>
		//! \param func Funktor generuj�cy warto�ci funkcji
		FunctionAccessor(Func && func, const bool even = false,
			const bool odd = false, const MonotonyType monotony = NonMonotonic)
			: BaseType(even, odd, monotony), f_(std::forward<Func>(func)) {}

		//! Destruktor
		virtual ~FunctionAccessor() {}
		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return f_(argument); }

	private:
		//! Funktor realizuj�cy ci�g�� funkcj� - ci�g�� w sensie daje warto�ci dla ka�dej warto�ci zadanego typu argumentu
		std::function<Ret(Arg)> f_;
	};

	//! \tparam Ret Typ warto�ci zwracanej z funckji
	//! \tparam Arg Typ warto�ci argumentu funkcji
	//! \tparam Object Typ obiektu kt�ry dostarcza danych
	template<typename Ret, typename Arg, typename Object>
	//! Implementacja wrappera dla funkcji ci�g�ej
	class FunctionAccessor<Ret(Object::*)(Arg)const> : public FunctionAccessorBase<Ret, Arg>
	{
	public:

		using BaseType = FunctionAccessorBase<Ret, Arg>;

		using value_type = typename BaseType::value_type;
		using argument_type = typename BaseType::argument_type;
		using sample_type = typename BaseType::sample_type;

	public:
		//! \param obj Obiekt na kt�rym wo�amy funkcj�
		FunctionAccessor(const Object & obj, Ret(Object::*const func)(Arg)const,
			const bool even = false, const bool odd = false,
			const MonotonyType monotony = NonMonotonic)
			: BaseType(even, odd, monotony), obj(obj), func(func) {}

		//! Destruktor
		virtual ~FunctionAccessor() {}
		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return (obj.*func)(argument); }

	private:
		//! Obiekt
		const Object & obj;
		//! Metoda
		Ret(Object::* const func)(Arg)const;
	};

	//! \tparam Ret Typ warto�ci zwracanej z funckji
	//! \tparam Arg Typ warto�ci argumentu funkcji
	//! \tparam Object Typ obiektu kt�ry dostarcza danych
	template<typename Ret, typename Arg, typename Object>
	//! Implementacja wrappera dla funkcji ci�g�ej
	using DataFunctionAccessor = SafeAccessorWrapper<FunctionAccessor<Ret(Object::*)(Arg)const>, Object>;
}

#endif	// __HEADER_GUARD_DATAACCESSOR__WRAPPERHELPERS_H__
