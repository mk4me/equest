/********************************************************************
created:  2016/01/20
filename: ValueCarrier.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__VALUECARRIER_H__
#define __HEADER_GUARD_UTILS__VALUECARRIER_H__

#include <utils/MemberExtractor.h>

namespace utils
{
	//! \tparam U Typ dla kt�rego chcemy dosta� ekstraktor
	template<typename U>
	//! Typ domy�lnego ekstraktora
	using ValueCarrierDefaultExtractor = typename std::conditional<is_like_smart_pointer<U>::value, PointerExtractor, TransparentExtractor>::type;

	//TODO - doda� obs�ug� go�ych wska�nik�w? Czy ma to sens? Co z nimi robi�?
	// kopiowa� obiekty pod nimi? Trzyma� tylko wska�niki?
	//! \tparam T Docelowy typ przechowywanych danych
	template<typename T>
	//! Klasa realizuje bezpieczne przechowywanie danych
	class ValueCarrier
	{
	public:
		//! Typ referencji jakiej dostarczamy
		using type = T;
		//! Typ referencji jakiej dostarczamy
		using const_ref_type = typename utils::add_const_reference<type>::type;

	private:

		//! Klasa bazowa gwarantuj�ca dost�p do danych
		class ValueCarrierBase
		{
		public:
			//! Destruktor wirtualny
			virtual ~ValueCarrierBase() {}
			//! \return Referencja do przechowywanej warto�ci
			virtual const_ref_type ref() const = 0;
		};

		template<typename U, typename E>
		//! Specjalizacja dla prostego przechowywania przez warto��
		class ValueCarrierImpl : public ValueCarrierBase, private E
		{
		public:
			//! Perfect forwarding penalty
			template<typename UU, typename EE = E>
			//! \param value Kopiowana warto�� do przechowania
			//! \param e Extractor
			ValueCarrierImpl(UU && value, EE && e = EE()) : E(std::forward<EE>(e)), value_(std::forward<UU>(value)) {}
			//! Destruktor wirtualny
			virtual ~ValueCarrierImpl() {}
			//! \return Referencja do przechowywanej warto�ci
			virtual const_ref_type ref() const override final { return this->E::extract(value_); }

		private:
			//! Przechowywana warto��
			const U value_;
		};

		//! \tparam U Typ przechowywany w tablicy
		//! \tparam N Rozmiar tablicy
		template<typename U, std::size_t N>
		//! Specjalizacja dla prostego przechowywania przez warto�� dla tablic
		class ValueCarrierImpl<U[N], TransparentExtractor> : public ValueCarrierBase
		{
		public:
			//! Typ tablicy
			using LC = U[N];

		private:
			//! \param value Tablica kt�r� chcemy skopiowa�
			//! \param dummy Statyczne rozpoznanie POD�w
			//! \return Wska�nik do skopiowanej tablicy
			static U* init(const LC & value, std::true_type)
			{
				utils::unique_ptr<LC> ret(new LC);
				std::memcpy(ret.get(), &value, sizeof(LC));
				return ret.release();
			}

			//! \param value Tablica kt�r� chcemy skopiowa�
			//! \param dummy Statyczne rozpoznanie POD�w
			//! \return Wska�nik do skopiowanej tablicy
			static U* init(const LC & value, std::false_type)
			{
				utils::unique_ptr<LC> ret(new LC);
				static_assert(std::is_default_constructible<U>::value && std::is_copy_assignable<U>::value, "Type U must be default constructible and trivially assignable");
				auto a = ret.get();
				for (auto & val : value)
				{
					*a = val;
					++a;
				}

				return ret.release();
			}

		public:

			//! \param value Kopiowana tablica
			ValueCarrierImpl(const LC & value, TransparentExtractor && e) : value_(init(value, std::is_pod<U>::value)) {}
			//! Destruktor wirtualny
			virtual ~ValueCarrierImpl() {}
			//! \return Referencja do przechowywanej warto�ci
			virtual const_ref_type ref() const override final { return *(value_.get()); }

		private:
			//! Przechowywana tablica (przydzielana dynamicznie)
			const utils::unique_ptr<const LC> value_;
		};

	public:

		//TODO
		//compile time warning dla kombinacji kopiowanie/move + member!!
		//bez sensu trzyma� jeszcze co� z otoczk�, skoro ca�a reszta nas nie interesuje

		//! \tparam U Typ no�nika danych docelowych
		//! \tparam dummy Warunki mo�liwo�ci konwersji
		template<typename U, typename E = ValueCarrierDefaultExtractor<typename utils::remove_toplevel<U>::type>>//, ENABLE_IF(!std::is_pointer<U>::value && (std::is_same<typename remove_toplevel<decltype(std::declval<const E>().extract(std::declval<const U>()))>::type, type>::value || std::is_base_of<type, typename remove_toplevel<decltype(std::declval<const E>().extract(std::declval<const U>()))>::type>::value))>
																												 //! \param value Kopiowany no�nik danych
		ValueCarrier(U && value, E && e = E()) : valueCarier_(new ValueCarrierImpl<U, E>(std::forward<U>(value), std::forward<E>(e))) {}
		//! Konstruktor kopiuj�cy
		//! \param Other Kopiowany no�nik
		ValueCarrier(const ValueCarrier & Other) : valueCarier_(Other.valueCarier_) {}
		//! Konstruktor przenosz�cy
		//! \param Other Kopiowany no�nik
		ValueCarrier(ValueCarrier && Other) : valueCarier_(std::move(Other.valueCarier_)) {}
		//! Destruktor
		~ValueCarrier() {}
		//! \return Referencja do przechowywanej warto�ci
		inline const_ref_type ref() const { return valueCarier_->ref(); }

	private:
		//! Owrapowane dane
		const utils::shared_ptr<const ValueCarrierBase> valueCarier_;
	};
}

#endif  // __HEADER_GUARD_UTILS__VALUECARRIER_H__
