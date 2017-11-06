/********************************************************************
	created:  2015/07/29	14:29:57
	filename: Interpolators.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__INTERPOLATORS_H__
#define __HEADER_GUARD_DATAACCESSOR__INTERPOLATORS_H__

#include <cmath>
#include <dataaccessorlib/Accessors.h>

namespace dataaccessor
{
	//! Manipulator danych bazuj�cy na operatorach typu. Interpoluje liniowo.
	struct LerpInterpolator
	{
		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argument�w		
		template <class ValueType, class ArgumentType>
		//! \param accessor Zwracana interpolowana warto��
		//! \param argument pr�bka dla czasu poni�ej lub identycznego
		//! \param startIdx pr�bka dla czasu powyzej lub identycznego
		//! \param endIdx time Warto�� okre�laj�ca procent przesuniecia wzgl�dem pr�bki poni�ej
		inline static ValueType interpolate(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & argument, const std::size_t startIdx, const std::size_t endIdx)
		{			
			const auto startSample = accessor.sample(startIdx);
			const auto endSample = accessor.sample(endIdx);
			const auto mu = (argument - startSample.first) / (endSample.first - startSample.first);
			
			return startSample.second * (1.0 - mu) + endSample.second * mu;
		}
	};

	//! Manipulator danych bazuj�cy na operatorach typu. Interpoluje liniowo.
	struct CosineInterpolator
	{
		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argument�w		
		template <class ValueType, class ArgumentType>
		//! \param accessor Zwracana interpolowana warto��
		//! \param argument pr�bka dla czasu poni�ej lub identycznego
		//! \param startIdx pr�bka dla czasu powyzej lub identycznego
		//! \param endIdx time Warto�� okre�laj�ca procent przesuniecia wzgl�dem pr�bki poni�ej
		inline static ValueType interpolate(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & argument, const std::size_t startIdx, const std::size_t endIdx)
		{
			static const auto PI = std::atan(1.0)*4.0;

			const auto startSample = accessor.sample(startIdx);
			const auto endSample = accessor.sample(endIdx);
			const auto mu = (argument - startSample.first) / (endSample.first - startSample.first);
			const auto mu2 = (1.0 - std::cos(mu * PI)) / 2.0;
			return startSample.second * (1.0 - mu2) + endSample.second * mu2;
		}
	};

	//! Manipulator danych bazuj�cy na operatorach typu. Interpoluje liniowo.
	struct QubicInterpolator
	{
		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argument�w		
		template <class ValueType, class ArgumentType>
		//! \param accessor Zwracana interpolowana warto��
		//! \param argument pr�bka dla czasu poni�ej lub identycznego
		//! \param startIdx pr�bka dla czasu powyzej lub identycznego
		//! \param endIdx time Warto�� okre�laj�ca procent przesuniecia wzgl�dem pr�bki poni�ej
		inline static ValueType interpolate(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & argument, const std::size_t startIdx, const std::size_t endIdx)
		{
			const bool startOK = (startIdx > 0);
			const bool endOK = (endIdx < (accessor.size() - 1));
			UTILS_ASSERT(startOK || endOK, "Jeden z warunk�w musi by� poprawny");

			const auto y1 = accessor.sample(startIdx);
			const auto y2 = accessor.sample(endIdx);

			const auto mu = (argument - y1.first) / (y2.first - y1.first);
			const auto mu2 = std::pow(mu, 2.0);

			auto y0 = y1.second;
			auto y3 = y2.second;

			const auto a3 = y1.second;

			auto a0 = a3;
			auto a1 = a3;
			auto a2 = a3;

			if (startOK == true && endOK == true){

				y0 = accessor.value(startIdx-1);
				y3 = accessor.value(endIdx+1);

				a0 = y3 - y2.second - y0 + y1.second;
				a1 = y0 - y1.second - a0;
				a2 = y2.second - y0;

			} else if (startOK == true){
				y0 = accessor.value(startIdx - 1);				

				a0 = - y0 + y1.second;
				a1 = y0 - y1.second - a0;
				a2 = y2.second - y0;				
				
			} else {

				y3 = accessor.value(endIdx + 1);

				a0 = y3 - y2.second;
				a1 = -a0;
				a2 = y2.second - y0.second;
			}

			return a0 * (mu * mu2) + a1 * mu2 + a2 * mu + a3;
		}
	};

	//! Manipulator danych bazuj�cy na operatorach typu. Interpoluje liniowo.
	struct CatmullRomInterpolator
	{
		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argument�w		
		template <class ValueType, class ArgumentType>
		//! \param accessor Zwracana interpolowana warto��
		//! \param argument pr�bka dla czasu poni�ej lub identycznego
		//! \param startIdx pr�bka dla czasu powyzej lub identycznego
		//! \param endIdx time Warto�� okre�laj�ca procent przesuniecia wzgl�dem pr�bki poni�ej
		inline static ValueType interpolate(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & argument, const std::size_t startIdx, const std::size_t endIdx)
		{
			const bool startOK = (startIdx > 0);
			const bool endOK = (endIdx < (accessor.size() - 1));
			UTILS_ASSERT(startOK || endOK, "Jeden z warunk�w musi by� poprawny");

			const auto y1 = accessor.sample(startIdx);
			const auto y2 = accessor.sample(endIdx);

			const auto mu = (argument - y1.first) / (y2.first - y1.first);
			const auto mu2 = std::pow(mu, 2.0);

			auto y0 = y1.second;
			auto y3 = y2.second;

			if (startOK == true && endOK == true){
				y0 = accessor.value(startIdx - 1);
				y3 = accessor.value(endIdx + 1);
			}
			else if (startOK == true){
				y0 = accessor.value(startIdx - 1);
			}
			else {
				y3 = accessor.value(endIdx + 1);
			}

			const auto a0 = 0.5 * (y3 - y0) + 1.5 * (y1.second - y2.second);
			const auto a1 = y0 - 2.5 * y1.second + 2.0 * y2.second - 0.5 * y3;
			const auto a2 = 0.5 * (y2.second - y0);
			const auto a3 = y1.second;

			return a0 * (mu * mu2) + a1 * mu2 + a2 * mu + a3;
		}
	};

	//! Manipulator danych bazuj�cy na operatorach typu. Interpoluje liniowo.
	class HermitInterpolator
	{
	public:

		HermitInterpolator(const double tension, const double bias)
			: mul0((1.0 + bias)*(1.0 - tension) / 2.0),
			mul1((1.0 - bias)*(1.0 - tension) / 2.0)
		{
			if (tension < 0 || tension > 1 || bias < 0 || bias > 1){
				throw std::invalid_argument("Invalid interpolator configuration");
			}
		}

		HermitInterpolator(const HermitInterpolator & Other)
			: mul0(Other.mul0), mul1(Other.mul1)
		{

		}

		HermitInterpolator(HermitInterpolator && Other)
			: mul0(Other.mul0), mul1(Other.mul1)
		{
			
		}

		//! \tparam ValueType Typ warto�ci
		//! \tparam ArgumentType Typ argument�w		
		template <class ValueType, class ArgumentType>
		//! \param accessor Zwracana interpolowana warto��
		//! \param argument pr�bka dla czasu poni�ej lub identycznego
		//! \param startIdx pr�bka dla czasu powyzej lub identycznego
		//! \param endIdx time Warto�� okre�laj�ca procent przesuniecia wzgl�dem pr�bki poni�ej
		inline ValueType interpolate(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & argument, const std::size_t startIdx, const std::size_t endIdx) const
		{
			const bool startOK = (startIdx > 0);
			const bool endOK = (endIdx < (accessor.size() - 1));
			UTILS_ASSERT(startOK || endOK, "Jeden z warunk�w musi by� poprawny");

			const auto y1 = accessor.sample(startIdx);
			const auto y2 = accessor.sample(endIdx);

			const auto mu = (argument - y1.first) / (y2.first - y1.first);
			const auto mu2 = std::pow(mu, 2.0);
			const auto mu3 = mu2 * mu;

			const auto diffmu32mu23 = 2.0 * mu3 - 3.0 * mu2;

			const auto a0 = diffmu32mu23 + 1.0;
			const auto a2 = mu3 - mu2;
			const auto a1 = a2 - mu2 + mu;
			const auto a3 = -diffmu32mu23;			

			auto y0 = y1.second;
			auto y3 = y2.second;

			const auto y2y1diff = y2.second - y1.second;
			auto m1 = y2y1diff * mul0;
			auto m0 = y2y1diff * mul1;

			if (startOK == true && endOK == true){
				y0 = accessor.value(startIdx - 1);
				y3 = accessor.value(endIdx + 1);

				m0 += (y1.second - y0) * mul0;
				m1 += (y3 - y2.second) * mul1;

			}
			else if (startOK == true){
				y0 = accessor.value(startIdx - 1);

				m0 += (y1.second - y0) * mul0;				

			}
			else {
				y3 = accessor.value(endIdx + 1);
				
				m1 += (y3 - y2.second) * mul1;
			}

			return a0 * y1.second + a1 * m0 + a2 * m1 + a3 * y2.second;
		}

	private:
		const double mul0;
		const double mul1;
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__INTERPOLATORS_H__
