/********************************************************************
created:  2012/04/26
created:  26:4:2012   23:47
filename: Accessors.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD_DATAACCESSOR__ACCESSORS_H__
#define HEADER_GUARD_DATAACCESSOR__ACCESSORS_H__

#include <dataaccessorlib/Types.h>
#include <dataaccessorlib/Features.h>
#include <utils/TypeInfo.h>
#include <utils/SmartPtr.h>
#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/pointer_traits.h>
#include <map>

namespace dataaccessor
{
	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych kanału w formie ciągłej
	class IFunctionAccessorT;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do danych kanału
	class IDiscreteAccessorT;

	//! Klasa bazowa wszystkich akcesorów
	class IAccessor
	{
	protected:

		//! Domyśłny konstruktor
		IAccessor() {}
		//! Konstruktor kopiujący
		//! \param other Kopiowany obiekt
		IAccessor(const IAccessor & other) : features(other.features) {}
		//! Konstruktor przenoszący
		//! \param other Przenoszony obiekt
		IAccessor(IAccessor && other) : features(std::move(other.features)) {}

	public:
		//! Desturktor wirtualny
		virtual ~IAccessor() {}

		//! \param id Identyfikator cechy
		//! \return Czy cecha istnieje
		inline bool featureExists(const int id) const { return features.find(id) != features.end(); }

		//! \param type Typ cechy o jaką pytamy
		//! \return Cecha o zadanym typie, nullptr jeśli nie ma takiej cechy
		inline IFeaturePtr feature(const int id) const
		{
			auto it = features.find(id);
			if (it != features.end()) {
				return it->second;
			}

			return IFeaturePtr();
		}

		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<typename FeatureT,
			ENABLE_IF(std::is_base_of<IFeature, FeatureT>::value)>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<FeatureT> feature(FeatureT * dummy = nullptr) const
		{
			auto f = this->feature(FeatureT::ID);
			if (f != nullptr) {
				//Dodatkowe zabezpieczenie przed pobraniem cechy dla innego kanału (cecha może mieć dobre ID ale dotyczyć innych typów)
				return utils::dynamic_pointer_cast<FeatureT>(f);
			}

			return utils::shared_ptr<FeatureT>();
		}

		//! \param feature Cecha dopinana do akcesora
		//! \return Czy udało się dopiąć cechę czy nie - false kiedy cecha o danym typie już jest podpięta
		bool attachFeature(IFeaturePtr feature) const
		{
			if (feature == nullptr || !type().match(feature->type())) {
				return false;
			}

			auto it = features.find(feature->id());
			if (it == features.end()) {
				features.insert({ feature->id(), feature });
				return true;
			}

			return false;
		}

		//! \param type Cecha którą chcemy usunąć
		inline bool removeFeature(const int id) { return (features.erase(id) > 0); }

		//! \tparam FeatureT Cecha jaką chcemy usunąć
		//! \tparam dummy
		template<typename FeatureT,
			ENABLE_IF(std::is_base_of<IFeature, FeatureT>::value)>
			//! \param dummy
			//! \return Czy cecha została usunięta (była obecna)
			inline bool removeFeature(FeatureT * dummy = nullptr) { return removeFeature(FeatureT::ID); }

		//! \return Typ akcesora
		virtual AccessorType type() const = 0;

	private:
		//! Mapa cech kanału
		mutable std::map<int, IFeaturePtr> features;
	};

	//! \tparam ValueType Typ wartości akcesora dla których przechowujemy własności
	template<typename ValueType>
	//! Interfejs akcesora cech dla wartości
	class IValueAccessorT : public virtual IAccessor
	{
	public:
		//! Typ przechowywanych wartości
		using value_type = ValueType;
		//! Typ cechy wartości
		using ValueFeature = IValueFeatureT<value_type>;

	public:

		using IAccessor::feature;

		//! Destruktor wirtualny
		virtual ~IValueAccessorT() {}

		//! \return Typ akcesora
		inline static AccessorType myType() { return AccessorType(typeid(value_type), typeid(void)); }

		//! \tparam ValueFeatureT Typ cechy wartości o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ValueFeatureT,
			ENABLE_IF(std::is_base_of<ValueFeature, ValueFeatureT<value_type>>::value)>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ValueFeatureT<value_type>> feature(ValueFeatureT<value_type> * dummy = nullptr) const { return this->IAccessor::feature<ValueFeatureT<value_type>>(); }
	};

	//! \tparam ArgumentType Typ argumentów akcesora dla których przechowujemy własności
	template<typename ArgumentType>
	//! Intefejs akcesora dla cech argumentów
	class IArgumentAccessorT : public virtual IAccessor
	{
	public:
		//! Typ argumentów
		using argument_type = ArgumentType;
		//! Typ cechy argumentów
		using ArgumentFeature = IArgumentFeatureT<argument_type>;

	public:

		using IAccessor::feature;

		//! Destruktor wirtualny
		virtual ~IArgumentAccessorT() {}

		//! \return Typ akcesora
		inline static AccessorType myType() { return AccessorType(typeid(void), typeid(argument_type)); }

		//! \tparam ArgumentFeatureT Typ cechy argumentów o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ArgumentFeatureT,
			ENABLE_IF(std::is_base_of<ArgumentFeature, ArgumentFeatureT<argument_type>>::value)>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ArgumentFeatureT<argument_type>> feature(ArgumentFeatureT<argument_type> * dummy = nullptr) const { return this->IAccessor::feature<ArgumentFeatureT<argument_type>>(); }
	};

	namespace impl
	{
		template<typename T, typename U>
		static inline void set(T & smartPtr, U * ptr, std::true_type)
		{
			smartPtr.reset(ptr);
		}

		template<typename T, typename U>
		static inline void set(T & smartPtr, const U & ptr, std::false_type)
		{
			smartPtr = ptr;
		}

		template<typename T, typename U>
		static inline void set(T & smartPtr, const U & ptr)
		{
			set(smartPtr, ptr, std::is_pointer<U>());
		}
	}

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs akcesora cech wartości i argumentów
	class IAccessorT : public virtual IAccessor,
		public virtual IValueAccessorT<ValueType>,
		public virtual IArgumentAccessorT<ArgumentType>
	{
	public:

		//! Próbka: argument + wartość
		using sample_type = std::pair<typename IArgumentAccessorT<ArgumentType>::argument_type, typename IValueAccessorT<ValueType>::value_type>;
		//! Typ akcesorta dyskretnego
		using DiscreteAccessor = IDiscreteAccessorT<typename IValueAccessorT<ValueType>::value_type, typename IArgumentAccessorT<ArgumentType>::argument_type>;
		//! Typ akcesora dyskretnego
		using FunctionAccessor = IFunctionAccessorT<typename IValueAccessorT<ValueType>::value_type, typename IArgumentAccessorT<ArgumentType>::argument_type>;
		//! Typ cechy akcesora
		using AccessorFeature = IAccessorFeatureT<typename IValueAccessorT<ValueType>::value_type, typename IArgumentAccessorT<ArgumentType>::argument_type>;

	public:
		//! Destruktor wirtualny
		virtual ~IAccessorT() {}
		//! \return Kanał jako dyskretny
		virtual DiscreteAccessor * asDiscrete() { return nullptr; };
		//! \return Kanał jako dyskretny
		virtual const DiscreteAccessor * asDiscrete() const { return nullptr; };
		//! \return Kanał jako ciągły
		virtual FunctionAccessor * asFunction() { return nullptr; };
		//! \return Kanał jako ciągły
		virtual const FunctionAccessor * asFunction() const { return nullptr; };
		//! \return Identyfikator kanału
		inline static AccessorType myType() { return AccessorType(typeid(typename IValueAccessorT<ValueType>::value_type), typeid(typename IArgumentAccessorT<ArgumentType>::argument_type)); }
		//! \return Identyfikator kanału
		virtual AccessorType type() const override final { return myType(); }

		using IAccessor::feature;
		using IValueAccessorT<ValueType>::feature;
		using IArgumentAccessorT<ArgumentType>::feature;

		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<typename FeatureT,
			ENABLE_IF(std::is_base_of<IFeature, FeatureT>::value)>// && utils::is_general_pointer<decltype(FeatureT::create((DiscreteAccessor*)0, (FunctionAccessor*)0))>::value)>
																  //! \param dummy
																  //! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline auto getOrCreateFeature(FeatureT * dummy = nullptr) const -> decltype(utils::shared_ptr<FeatureT>(FeatureT::create(asDiscrete(), asFunction())))
		{
			utils::shared_ptr<FeatureT> ret = this->IAccessor::feature<FeatureT>();

			if (ret == nullptr) {
				UTILS_ASSERT((asDiscrete() != nullptr) || (asFunction() != nullptr));
				impl::set(ret, FeatureT::create(asDiscrete(), asFunction()));
				this->IAccessor::attachFeature(ret);
			}

			return ret;
		}

		//! \tparam ValueFeatureT Typ cechy wartości o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ValueFeatureT,
			ENABLE_IF(std::is_base_of<typename IValueAccessorT<ValueType>::ValueFeature, ValueFeatureT<typename IValueAccessorT<ValueType>::value_type>>::value),
			typename VFT = ValueFeatureT<typename IValueAccessorT<ValueType>::value_type >>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline auto getOrCreateFeature(VFT * dummy = nullptr) const -> decltype(utils::shared_ptr<VFT>(VFT::create(this->asDiscrete(), this->asFunction()))) { return this->template getOrCreateFeature<VFT>(); }

		//! \tparam ArgumentFeatureT Typ cechy argumentów o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ArgumentFeatureT,
			ENABLE_IF(std::is_base_of<typename IArgumentAccessorT<ArgumentType>::ArgumentFeature, ArgumentFeatureT<typename IArgumentAccessorT<ArgumentType>::argument_type>>::value),
			typename AFT = ArgumentFeatureT<typename IArgumentAccessorT<ArgumentType>::argument_type >>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline auto getOrCreateFeature(AFT * dummy = nullptr) const -> decltype(utils::shared_ptr<AFT>(AFT::create(this->asDiscrete(), this->asFunction()))) { return this->template getOrCreateFeature < AFT>(); }

		//! \tparam AccessorFeatureT Typ cechy o jaką pytamy
		template<template<typename, typename> class AccessorFeatureT,
			ENABLE_IF(std::is_base_of<AccessorFeature, AccessorFeatureT<typename IValueAccessorT<ValueType>::value_type, typename IArgumentAccessorT<ArgumentType>::argument_type>>::value),
			typename AFT = AccessorFeatureT<typename IValueAccessorT<ValueType>::value_type, typename IArgumentAccessorT<ArgumentType>::argument_type >>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline auto feature(AFT * dummy = nullptr) const -> decltype(this->template feature < AFT>()) { return this->template feature < AFT>(); }

		//! \tparam AccessorFeatureT Typ cechy danych o jaką pytamy
		template<template<typename, typename> class AccessorFeatureT,
			ENABLE_IF(std::is_base_of<AccessorFeature, AccessorFeatureT<typename IValueAccessorT<ValueType>::value_type, typename IArgumentAccessorT<ArgumentType>::argument_type>>::value),
			typename AFT = AccessorFeatureT<typename IValueAccessorT<ValueType>::value_type, typename IArgumentAccessorT<ArgumentType>::argument_type >>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli istnieje cecha o danym id ale to nie ta o jaką pytamy
			inline utils::shared_ptr<AFT> getOrCreateFeature(AFT * dummy = nullptr) const { return this->template getOrCreateFeature<AFT>(); }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora cech
	using AccessorPtr = utils::shared_ptr<IAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;
	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora cech
	using AccessorConstPtr = utils::shared_ptr<const IAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych akcesora w formie funkcji
	class IFunctionAccessorT : public IAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Typ mojego akcesora
		using accessor_type = IFunctionAccessorT<ValueType, ArgumentType>;

	public:
		//! Destruktor wirtualny
		virtual ~IFunctionAccessorT() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Wartość dla zadanego argumentu
		virtual typename accessor_type::value_type value(const typename accessor_type::argument_type & argument) const = 0;
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Próbka dla zadanego argumentu
		virtual typename accessor_type::sample_type sample(const typename accessor_type::argument_type & argument) const = 0;
		//! \return Kanał jako ciągły
		virtual typename accessor_type::FunctionAccessor * asFunction() override final { return this; };
		//! \return Kanał jako ciągły
		virtual const typename accessor_type::FunctionAccessor * asFunction() const override final { return this; };
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	using FunctionAccessorPtr = utils::shared_ptr<IFunctionAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	using FunctionAccessorConstPtr = utils::shared_ptr<const IFunctionAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	class IGeneratedFunctionAccessorT : public IFunctionAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Typ mojego akcesora
		using accessor_type = IFunctionAccessorT<ValueType, ArgumentType>;
	public:
		//! Destruktor wirtualny
		virtual ~IGeneratedFunctionAccessorT() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Próbka dla zadanego argumentu
		virtual typename accessor_type::sample_type sample(const typename accessor_type::argument_type & argument) const override { return std::make_pair(argument, this->value(argument)); }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	class IOptimizedFunctionAccessorT : public IFunctionAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Typ mojego akcesora
		using accessor_type = IFunctionAccessorT<ValueType, ArgumentType>;
	public:
		//! Destruktor wirtualny
		virtual ~IOptimizedFunctionAccessorT() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Wartość dla zadanego argumentu
		virtual typename accessor_type::value_type value(const typename accessor_type::argument_type & argument) const override { return this->sample(argument).second; }
	};

	//! Baza kanałów dyskretnych
	class IDiscreteAccessor : public virtual IAccessor
	{
	public:
		//! Typ ilości elementów w kanale
		using size_type = std::size_t;

	public:
		//! Destruktor wirtualny
		virtual ~IDiscreteAccessor() {}

		//! \return Ilość próbek w kanale
		virtual size_type size() const = 0;

		//! \return Czy kanał nie zawiera danych
		virtual bool empty() const { return (size() == 0); }
	};


	//! \tparam ValueType Typ wartości kanału danych	
	template<typename ValueType>
	//! Interfejs opisujący dyskretny dostęp do wartości akcesora
	class IDiscreteValueAccessorT : public virtual IDiscreteAccessor,
		public virtual IValueAccessorT<ValueType>
	{
	public:

		using ValueFeature = typename IValueAccessorT<ValueType>::ValueFeature;

	public:

		using IValueAccessorT<ValueType>::feature;

		//! Destruktor wirtualny
		virtual ~IDiscreteValueAccessorT() {}
		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual typename IValueAccessorT<ValueType>::value_type value(const size_type idx) const = 0;

		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ValueFeatureT,
			ENABLE_IF(std::is_base_of<ValueFeature, ValueFeatureT<ValueType>>::value)>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline auto getOrCreateFeature(ValueFeatureT<ValueType> * dummy = nullptr) const -> decltype(utils::shared_ptr<ValueFeatureT<ValueType>>(ValueFeatureT<ValueType>::create(std::declval<IDiscreteValueAccessorT<ValueType>>())))
		{
			utils::shared_ptr<ValueFeatureT<ValueType>> ret = this->IAccessor::feature<ValueFeatureT<ValueType>>();

			if (ret == nullptr) {
				impl::set(ret, ValueFeatureT<ValueType>::create(*this));
				this->IAccessor::attachFeature(ret);
			}

			return ret;
		}
	};

	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do argumentów akcesora
	class IDiscreteArgumentAccessorT : public virtual IDiscreteAccessor,
		public virtual IArgumentAccessorT<ArgumentType>
	{
	public:

		using ArgumentFeature = typename IArgumentAccessorT<ArgumentType>::ArgumentFeature;

	public:

		using IArgumentAccessorT<ArgumentType>::feature;

		//! Destruktor wirtualny
		virtual ~IDiscreteArgumentAccessorT() {}
		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual typename IArgumentAccessorT<ArgumentType>::argument_type argument(const size_type idx) const = 0;

		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ArgumentFeatureT,
			ENABLE_IF(std::is_base_of<ArgumentFeature, ArgumentFeatureT<ArgumentType>>::value)>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline auto getOrCreateFeature(ArgumentFeatureT<ArgumentType> * dummy = nullptr) const -> decltype(utils::shared_ptr<ArgumentFeatureT<ArgumentType>>(ArgumentFeatureT<ArgumentType>::create(std::declval<IDiscreteArgumentAccessorT<ArgumentType>>())))
		{
			utils::shared_ptr<ArgumentFeatureT<ArgumentType>> ret = this->IAccessor::feature<ArgumentFeatureT<ArgumentType>>();

			if (ret == nullptr) {
				impl::set(ret, ArgumentFeatureT<ArgumentType>::create(*this));
				this->IAccessor::attachFeature(ret);
			}

			return ret;
		}
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny akcesor danych
	class IDiscreteAccessorT : public IAccessorT<ValueType, ArgumentType>,
		public IDiscreteValueAccessorT<ValueType>,
		public IDiscreteArgumentAccessorT<ArgumentType>
	{
	public:
		//! Typ mojego akcesora
		using accessor_type = IDiscreteAccessorT<ValueType, ArgumentType>;

	public:

		using IAccessorT<ValueType, ArgumentType>::feature;

		using IAccessorT<ValueType, ArgumentType>::getOrCreateFeature;
		using IDiscreteValueAccessorT<ValueType>::getOrCreateFeature;
		using IDiscreteArgumentAccessorT<ArgumentType>::getOrCreateFeature;
		using size_type = IDiscreteAccessor::size_type;


		//! Wirtualny destruktor
		virtual ~IDiscreteAccessorT() {};

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual typename accessor_type::sample_type sample(const size_type idx) const = 0;

		//! \return Kanał jako dyskretny
		virtual typename accessor_type::DiscreteAccessor * asDiscrete() override final { return this; };
		//! \return Kanał jako dyskretny
		virtual const typename accessor_type::DiscreteAccessor * asDiscrete() const override final { return this; };
	};

	//! \tparam ValueType Typ wartości akcesora
	//! \tparam ArgumentType Typ argumentu akcesora
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora z danymi dyskretnymi
	using DiscreteAccessorPtr = utils::shared_ptr<IDiscreteAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;
	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora z danymi dyskretnymi
	using DiscreteAccessorConstPtr = utils::shared_ptr<const IDiscreteAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny akcesor danych
	class IIndependentDiscreteAccessorT : public IDiscreteAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Typ mojego akcesora
		using accessor_type = IDiscreteAccessorT<ValueType, ArgumentType>;
	public:
		//! Wirtualny destruktor
		virtual ~IIndependentDiscreteAccessorT() {};

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual typename accessor_type::sample_type sample(const typename accessor_type::size_type idx) const override { return std::make_pair(this->argument(idx), this->value(idx)); }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny akcesor danych
	class IOptimizedDiscreteAccessorT : public IDiscreteAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Typ mojego akcesora
		using accessor_type = IDiscreteAccessorT<ValueType, ArgumentType>;
	public:
		//! Wirtualny destruktor
		virtual ~IOptimizedDiscreteAccessorT() {};

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual typename accessor_type::argument_type argument(const typename accessor_type::size_type idx) const override { return this->sample(idx).first; }

		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual typename accessor_type::value_type value(const typename accessor_type::size_type idx) const override { return this->sample(idx).second; }
	};
}

#endif	//	HEADER_GUARD_DATAACCESSOR__ACCESSORS_H__
