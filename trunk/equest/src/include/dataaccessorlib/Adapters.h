/********************************************************************
created:  2015/07/28	15:50:12
filename: Adapter.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__ADAPTER_H__
#define __HEADER_GUARD_DATAACCESSOR__ADAPTER_H__

#include <dataaccessorlib/UniformArgumentsGenerator.h>
#include <dataaccessorlib/NearestArgumentsFinder.h>
#include <dataaccessorlib/SafeAccessorWrapper.h>
#include <dataaccessorlib/BoundedArgumentsFeature.h>
#include <dataaccessorlib/BoundedValuesFeature.h>
#include <dataaccessorlib/FunctionFeature.h>
#include <dataaccessorlib/FunctionDescriptionFeature.h>
#include <dataaccessorlib/Interpolators.h>
#include <dataaccessorlib/Extrapolators.h>
#include <utils/MemberExtractor.h>

namespace dataaccessor
{
	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam ArgumentsGenerator Generator argument�w dla kolejnych indeks�w
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator < ArgumentType >>
		//! Klasa realizuje dyskretny dost�p do ci�g�ego kana�u danych opisuj�cego funkcj�
	class FunctionDiscreteAccessorAdapter : public IOptimizedDiscreteAccessorT<ValueType, ArgumentType>,
		private ArgumentsGenerator
	{
	public:

		using sample_type = typename IOptimizedDiscreteAccessorT<ValueType, ArgumentType>::sample_type;
		using size_type = typename IOptimizedDiscreteAccessorT<ValueType, ArgumentType>::size_type;

	public:
		template<typename AG = ArgumentsGenerator>
		//! \param accessor Kana� wrapowany		
		//! \param argumentsGenerator Generator argument�w
		FunctionDiscreteAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			AG && argumentsGenerator = AG())
			: ArgumentsGenerator(std::forward<AG>(argumentsGenerator)), accessor(accessor)
		{
			this->attachFeature(accessor.template getOrCreateFeature<FunctionFeature>());
			//TODO
			//na bazie argumentsGenerator doda� boundedarguments feature
		}

		//! Destruktor wirtualny
		virtual ~FunctionDiscreteAccessorAdapter() {}

		//! \param idx Indeks pr�bki
		//! \return Pr�bka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{
			return accessor.sample(this->ArgumentsGenerator::argument(idx));
		}

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return this->ArgumentsGenerator::size(); }

	private:
		//! Kana� ci�g�y
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam ArgumentsGenerator Generator argument�w dla kolejnych indeks�w
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator < ArgumentType >>
		//! Klasa realizuje dyskretny dost�p do ci�g�ego kana�u danych opisuj�cego funkcj�
		using SafeFunctionDiscreteAccessorAdapter = SafeAccessorWrapper < FunctionDiscreteAccessorAdapter < ValueType, ArgumentType, ArgumentsGenerator >,
		IFunctionAccessorT < ValueType, ArgumentType >> ;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam Interpolator Obiekt realizuj�cy interpolacj� pomi�dzy pr�bkami kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator < ValueType >>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
	class DiscreteFunctionAccessorAdapter : public IGeneratedFunctionAccessorT<ValueType, ArgumentType>,
		private Interpolator, private Extrapolator
	{
	public:

		template<typename IT = Interpolator, typename ET = Extrapolator>
		//! \param accessor Kana� wrapowany
		//! \param interpolator Obiekt interpoluj�cy wewn�trz kana�u
		//! \param extrapolator Obiekt ekstrapoluj�cy poza kana�em
		DiscreteFunctionAccessorAdapter(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			IT && interpolator = IT(), ET && extrapolator = ET())
			: Interpolator(std::forward<IT>(interpolator)), Extrapolator(std::forward<ET>(extrapolator)), accessor(accessor),
			range(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessorT<ValueType, ArgumentType>&,
				const ArgumentType&)>(&NearestArgumentsFinder::range<ValueType, ArgumentType>))
		{
			auto ff = accessor.template getOrCreateFeature<FunctionFeature>();
			if (ff->isFunction() == false) {
				throw std::runtime_error("Non function data");
			}

			this->attachFeature(ff);

			feature = accessor.template getOrCreateFeature<BoundedArgumentsFeature>();

			auto uaf = accessor.template getOrCreateFeature<UniformArgumentsFeature>();
			if (uaf != nullptr) {
				range = std::bind(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessorT<ValueType, ArgumentType>&,
					const ArgumentType&, const ArgumentType&)>(&NearestArgumentsFinder::range<ValueType, ArgumentType>),
					std::placeholders::_1, std::placeholders::_2, uaf->argumentsInterval());
			}
		}

		//! Destruktor wirtualny
		virtual ~DiscreteFunctionAccessorAdapter() {}

		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		ValueType value(const ArgumentType & argument) const override
		{
			if (feature != nullptr) {

				if (argument < feature->minArgument()) {
					return this->Extrapolator::argumentUnderflow(argument, accessor);
				}
				else if (argument > feature->maxArgument()) {
					return this->Extrapolator::argumentOverflow(argument, accessor);
				}
				else {
					const auto r = range(accessor, argument);
					if (r.first == r.second) {
						return accessor.value(r.first);
					}
					else {
						return this->Interpolator::interpolate(accessor, argument, r.first, r.second);
					}
				}
			}
			else if (accessor.empty() == false) {
				const auto arg = accessor.argument(0);
				if (argument < arg) {
					return this->Extrapolator::argumentUnderflow(argument, accessor);
				}
				else if (argument > arg) {
					return this->Extrapolator::argumentOverflow(argument, accessor);
				}
				else {
					return accessor.value(0);
				}
			}
			else {
				throw std::range_error("Empty accessor");
			}

			return ValueType();
		}

	private:
		utils::shared_ptr<BoundedArgumentsFeature<ArgumentType>> feature;
		//! Wrapowany kana� danych
		const IDiscreteAccessorT<ValueType, ArgumentType> & accessor;
		//! Funkcja realizuj�ca pobieranie najbli�szych indeks�w
		std::function<NearestArgumentsFinder::Range(const IDiscreteAccessorT<ValueType, ArgumentType> &, const ArgumentType & argument)> range;
	};

	//! \tparam ChannelType Typ kana�u do zmiany reprezentacji dost�pu do danych
	//! \tparam Interpolator Obiekt realizuj�cy interpolacj� pomi�dzy pr�bkami kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator < ValueType >>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeDiscreteFunctionAccessorAdapter = SafeAccessorWrapper < DiscreteFunctionAccessorAdapter < ValueType, ArgumentType, Interpolator, Extrapolator >,
		IDiscreteAccessorT<ValueType, ArgumentType >> ;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Wrapper dla dyskretnych akcesor�w, zaw�aj�cy ich zakres
	class DiscreteSubAccessorAdapter : public IDiscreteAccessorT<ValueType, ArgumentType>,
		private Extrapolator
	{
	public:

		using argument_type = typename IDiscreteAccessorT<ValueType, ArgumentType>::argument_type;
		using value_type = typename IDiscreteAccessorT<ValueType, ArgumentType>::value_type;
		using sample_type = typename IDiscreteAccessorT<ValueType, ArgumentType>::sample_type;
		using size_type = typename IDiscreteAccessorT<ValueType, ArgumentType>::size_type;

	private:

		void init()
		{
			if (start_ + size_ > accessor.size()) {
				throw std::runtime_error("Invalid discrete subaccessor configuration");
			}

			this->attachFeature(accessor.template getOrCreateFeature<UniformArgumentsFeature>());

			auto ff = accessor.template getOrCreateFeature<FunctionFeature>();
			if (ff->isFunction() == true) {

				this->attachFeature(ff);

				auto fdf = accessor.template feature<FunctionDescriptionFeature>();

				if (fdf != nullptr && fdf->monotony() != NonMonotonic) {
					auto minSample = accessor.sample(start_);
					auto maxSample = accessor.sample(start_ + size_);
					auto resArg = std::minmax(minSample.first, maxSample.first);
					auto resVal = std::minmax(minSample.second, maxSample.second);
					this->attachFeature(IFeaturePtr(new BoundedArgumentsFeature<ArgumentType>(resArg.first, resArg.second)));
					this->attachFeature(IFeaturePtr(new BoundedValuesFeature<ValueType>(resVal.first, resVal.second)));
				}
			}
		}

	public:

		template<typename ET = Extrapolator>
		//! \param accessor Wrapowany accessor
		//! \param start Index startowy
		//! \param size Ilo�c pr�bek od indeksu startowego
		//! \param extrapolator
		DiscreteSubAccessorAdapter(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const std::size_t start, const std::size_t size,
			ET && extrapolator = ET()) :
			Extrapolator(std::forward<ET>(extrapolator)), accessor(accessor),
			start_(start), size_(size)
		{
			init();
		}

		//! Destruktor wirtualny
		virtual ~DiscreteSubAccessorAdapter() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (ridx >= accessor.size()) {
				throw std::out_of_range("Argument out of range");
			}

			return accessor.argument(ridx);
		}

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (ridx >= accessor.size()) {
				throw std::out_of_range("Argument out of range");
			}

			if (idx >= size_) {
				return this->Extrapolator::argumentOverflow(accessor.argument(ridx), accessor);
			}
			else {
				return accessor.value(ridx);
			}
		}

		//! \param idx Indeks pr�bki
		//! \return Pr�bka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (idx < size_) {
				return accessor.sample(ridx);
			}
			else if (ridx < accessor.size()) {
				return{ accessor.argument(ridx), ((idx < size_) ? accessor.value(ridx) : this->Extrapolator::argumentOverflow(accessor.argument(ridx), accessor)) };
			}
			else {
				throw std::out_of_range("Argument out of range");
			}
		}

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return size_; }

	private:

		//! \param idx Indeks o jaki pytamy w tym kanale
		//! \return Mapowanie naszego indeksu na indeks wrapowanego kana�u
		inline std::size_t realIdx(const std::size_t idx) const { return start_ + idx; }

	private:
		//! Wrapowany kana�
		const IDiscreteAccessorT<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const std::size_t start_;
		//! Ilo�� pr�bek
		const std::size_t size_;
	};

	//! \tparam ChannelType Typ kana�u do zmiany reprezentacji dost�pu do danych
	//! \tparam Interpolator Obiekt realizuj�cy interpolacj� pomi�dzy pr�bkami kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeDiscreteSubAccessorAdapter = SafeAccessorWrapper <DiscreteSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >>;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Wrapper dla ci�g�ych akcesor�w, zaw�aj�cy ich zakres
	class FunctionSubAccessorAdapter : public IFunctionAccessorT<ValueType, ArgumentType>,
		private Extrapolator
	{
	public:

		using argument_type = typename IFunctionAccessorT<ValueType, ArgumentType>::argument_type;
		using value_type = typename IFunctionAccessorT<ValueType, ArgumentType>::value_type;
		using sample_type = typename IFunctionAccessorT<ValueType, ArgumentType>::sample_type;

	private:

		void init()
		{
			if (start > end) {
				throw std::invalid_argument("Invalid arguments for function subaccessor range");
			}

			this->attachFeature(accessor.template getOrCreateFeature<FunctionFeature>());

			auto fdf = accessor.template feature<FunctionDescriptionFeature>();

			if (fdf != nullptr && fdf->monotony() != NonMonotonic) {
				auto minSample = accessor.sample(start);
				auto maxSample = accessor.sample(end);
				auto resArg = std::minmax(minSample.first, maxSample.first);
				auto resVal = std::minmax(minSample.second, maxSample.second);
				this->attachFeature(IFeaturePtr(new BoundedArgumentsFeature<ArgumentType>(resArg.first, resArg.second)));
				this->attachFeature(IFeaturePtr(new BoundedValuesFeature<ValueType>(resVal.first, resVal.second)));
			}
			else {
				//TODO
				//jesli dodano valueboud lub argument bound to dodajemy po aktualizacji
			}
		}

	public:

		template<typename ET = Extrapolator>
		//! \param accessor Wrapowany accessor
		//! \param start Argument od kt�rego zaczynamy
		//! \param end Argument na kt�rym 
		//! \param extrapolator Extrapolator dla brzegowych warto�ci argument�w
		FunctionSubAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & start, const ArgumentType & end,
			ET && extrapolator = ET())
			: Extrapolator(std::forward<ET>(extrapolator)), accessor(accessor), start(start), end(end)
		{
			init();
		}

		//! Destruktor wirtualny
		virtual ~FunctionSubAccessorAdapter() {}

		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override
		{
			if (argument < start) {
				return this->Extrapolator::argumentUnderflow(argument, accessor);
			}
			else if (argument > end) {
				return this->Extrapolator::argumentOverflow(argument, accessor);
			}
			else {
				return accessor.value(argument);
			}
		}
		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Pr�bka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const override
		{
			if (argument < start) {
				return{ argument, this->Extrapolator::argumentUnderflow(argument, accessor) };
			}
			else if (argument > end) {
				return{ argument, this->Extrapolator::argumentOverflow(argument, accessor) };
			}
			else {
				return accessor.sample(argument);
			}
		}

	private:
		//! Wrapowany kana�
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const ArgumentType start;
		//! Ilo�� pr�bek
		const ArgumentType end;
	};

	//! \tparam ValueType Typ warto�ci
	//! \tparam ArgumentType Typ argument�w	
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeFunctionSubAccessorAdapter = SafeAccessorWrapper < FunctionSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >>;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor, typename ArgumentExtractor = utils::TransparentExtractor,
		typename DestValueType = typename std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = typename std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
		//! Wrapper dla dyskretnych akcesor�w, zmieniaj�cy ich reprezentacj�
	class DiscreteAccessorAdapter : public IIndependentDiscreteAccessorT<DestValueType, DestArgumentType>,
		private ValueExtractor, private ArgumentExtractor
	{
	public:

		using argument_type = typename IIndependentDiscreteAccessorT<DestValueType, DestArgumentType>::argument_type;
		using value_type = typename IIndependentDiscreteAccessorT<DestValueType, DestArgumentType>::value_type;
		using size_type = typename IIndependentDiscreteAccessorT<DestValueType, DestArgumentType>::size_type;

	public:

		template<typename VT = ValueExtractor, typename AT = ArgumentExtractor>
		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			VT && valueExtractor = VT(),
			AT && argumentExtractor = AT())
			: ValueExtractor(std::forward<VT>(valueExtractor)),
			ArgumentExtractor(std::forward<AT>(argumentExtractor)),
			accessor(accessor)
		{

		}

		//! Destruktor wirtualny
		virtual ~DiscreteAccessorAdapter() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return this->ArgumentExtractor::extract(accessor.argument(idx), idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return this->ValueExtractor::extract(accessor.value(idx), idx); }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return accessor.size(); }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return accessor.empty(); }

	private:
		//! Accessor
		const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor, typename ArgumentExtractor = utils::TransparentExtractor,
		typename DestValueType = typename std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = typename std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeDiscreteAccessorAdapter = SafeAccessorWrapper < DiscreteAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, ArgumentExtractor, DestValueType, DestArgumentType >,
		IDiscreteAccessorT < BaseValueType, BaseArgumentType >> ;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType, typename ValueExtractor,
		typename DestValueType = typename std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), std::declval<BaseArgumentType>()))>::type>
		//! Wrapper dla funkcji, zmieniaj�cy ich reprezentacj�
	class FunctionAccessorAdapter : public IGeneratedFunctionAccessorT<DestValueType, BaseArgumentType>,
		private ValueExtractor
	{
	public:

		using argument_type = typename IGeneratedFunctionAccessorT<DestValueType, BaseArgumentType>::argument_type;
		using value_type = typename IGeneratedFunctionAccessorT<DestValueType, BaseArgumentType>::value_type;

	public:

		template<typename VT = ValueExtractor>
		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		FunctionAccessorAdapter(const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor,
			VT && valueExtractor = VT())
			: ValueExtractor(std::forward<VT>(valueExtractor)), accessor(accessor)
		{
			this->attachFeature(accessor.template getOrCreateFeature<FunctionFeature>());
		}

		//! Destruktor wirtualny
		virtual ~FunctionAccessorAdapter() {}

		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return this->ValueExtractor::extract(accessor.value(argument), argument); }

	private:
		//! Accessor
		const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor,
		typename DestValueType = typename std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeFunctionAccessorAdapter = SafeAccessorWrapper < FunctionAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, DestValueType>>;
}

#endif	// __HEADER_GUARD_DATAACCESSOR__ADAPTER_H__
