/********************************************************************
	created:  2015/07/29	14:29:09
	filename: Extrapolators.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__EXTRAPOLATORS_H__
#define __HEADER_GUARD_DATAACCESSOR__EXTRAPOLATORS_H__

#include <dataaccessorlib/Interpolators.h>
#include <utils/Clamping.h>

namespace dataaccessor
{
	//! Typ obsuguj�cy zapytania o warto�ci kana�u dla czasu spoza zakresu pokrytego przez kana�
	//! Rzuca wyj�tkami
	class ExceptionExtrapolator
	{
	public:
		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \return Warto�� dla argumentu spoza kana�u
		inline static ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			throw std::runtime_error("Argument below allowed/available value");
			return ValueType();
		}

		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \return Warto�� dla argumentu spoza kana�u
		inline static ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			throw std::runtime_error("Argument over allowed/available value");
			return ValueType();
		}
	};

	//! \tparam Interpolator Typ interpolatora
	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType, typename Interpolator = LerpInterpolator>
	//! Klasa realizuj�ca okresowy obraz kana�u
	class PeriodicExtrapolator
	{
	public:

		//! Konstruktor
		//! \param minArg Minimalny argument
		//! \param maxArg Maksymalny argument
		//! \param Interpolator Interpolator u�ywany do interpolacji wewn�trz kana�u
		template<typename I = Interpolator>
		PeriodicExtrapolator(const ArgumentType & minArg, const ArgumentType & maxArg,
			I && interpolator = I())
			: interpolator(std::forward<I>(interpolator)), minArg(minArg), maxArg(maxArg),
			length(maxArg - minArg)
		{
			if (minArg >= maxArg){
				throw std::runtime_error("Invalid periodic extrapolator configuration");
			}
		}

		//! \param Other Kopiowany extrapolator okresowy
		PeriodicExtrapolator(const PeriodicExtrapolator & Other)
			: interpolator(Other.interpolator),	minArg(Other.minArg),
			maxArg(Other.maxArg), length(Other.length)
		{
		
		}

		//! \param Other przenoszony extrapolator okresowy
		PeriodicExtrapolator(PeriodicExtrapolator && Other)
			: interpolator(std::move(Other.interpolator)), minArg(Other.minArg),
			maxArg(Other.maxArg), length(Other.length)
		{
		
		}

		//! Destruktor
		~PeriodicExtrapolator() {}

		//! \tparam ValueType Typ warto�ci		
		template<typename ValueType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \param accessor Kana� dla kt�rego pytali�my o argument
		//! \return Warto�� dla argumentu spoza kana�u		
		inline ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor) const
		{
			const auto arg = utils::periodic_clamp_underflow(argument, minArg, maxArg, length);				
			return interpolator.interpolate(arg, accessor);
		}

		//! \tparam ValueType Typ warto�ci		
		template<typename ValueType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \param accessor Kana� dla kt�rego pytali�my o argument
		//! \return Warto�� dla argumentu spoza kana�u	
		inline ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor) const
		{
			const auto arg = utils::periodic_clamp_overflow(argument, minArg, maxArg, length);
			return interpolator.interpolate(arg, accessor);
		}

	private:
		//! Interpolator
		const Interpolator interpolator;
		//! Najmniejszy argument
		const ArgumentType minArg;
		//! Najwi�kszy argument
		const ArgumentType maxArg;
		//! D�ugo�� - max argument - min argument
		const ArgumentType length;
	};

	//! \tparam ValueType Typ warto�ci
	template<typename ValueType>
	//! Typ obsuguj�cy zapytania o warto�ci kana�u dla warto�ci spoza zakresu pokrytego przez kana�
	//! Powiela skrajne pr�bki
	class BorderExtrapolator
	{
	public:
		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argumentu
		template<typename ArgumentType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \param accessor Kana� dla kt�rego pytali�my o argument
		//! \return Warto�� dla argumentu spoza kana�u		
		static inline ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return accessor.value(0);
		}

		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argumentu
		template<typename ArgumentType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \param accessor Kana� dla kt�rego pytali�my o argument
		//! \return Warto�� dla argumentu spoza kana�u	
		static inline ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return accessor.value(accessor.size() - 1);
		}	
	};

	//! Typ realizuj�cy liniow� ekstrapolacj�
	class LinearExtrapolator
	{
	public:

		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \param accessor Kana� dla kt�rego pytali�my o argument
		//! \return Warto�� dla argumentu spoza kana�u		
		static inline ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return LerpInterpolator::interpolate(accessor, argument, 0, 1);
		}

		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument kt�ry nie mie�ci si� w dozwolnym zakresie
		//! \param accessor Kana� dla kt�rego pytali�my o argument
		//! \return Warto�� dla argumentu spoza kana�u	
		static inline ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			const auto s = accessor.size() - 2;
			return LerpInterpolator::interpolate(accessor, argument, s, s + 1);
		}
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__EXTRAPOLATORS_H__
