/********************************************************************
	created:  2016/07/18
	filename: NearestArgumentsFinder.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__NEARESTARGUMENTSFINDER_H__
#define __HEADER_GUARD_DATAACCESSOR__NEARESTARGUMENTSFINDER_H__

#include <dataaccessorlib/Accessors.h>

namespace dataaccessor
{
	//! Klasa pomocnicza przy znajdowaniu argument�w otaczaj�cych dany argument w dyskretnym kanale
	struct NearestArgumentsFinder
	{
		//! Typ zakresu indeks�w
		using Range = std::pair<std::size_t, std::size_t>;

		//! \tparam ArgumentType Typ argumentu o kt�ry pytamy
		//! \tparam ValueType Typ warto�ci
		template<typename ValueType, typename ArgumentType>
		//! \param accessor Kana� dyskretny w kt�rym szukamy otaczaj�cych pr�bek
		//! \param argument Argument wok� kt�rego szukamy pr�bek
		//! \return Zakres pr�bek otaczaj�cych dany argument (r�nica warto�ci indeks�w 1),
		//! kiedy pr�bka znajduje si� w kanale zakres o dw�ch identycznych indeksach (r�nica 0),
		//! kiedy pr�bka poza zakresem kana�u dowolny zakres indeks�w z r�nic� wieksz� ni� 1
		//! Ta implementacja dzia�a w czasie O(log(N)) - podzia� binarny bo dane sa u�o�one chronologicznie					
		static inline Range range(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & argument)
		{
			UTILS_COMPILER_WARNING("Unoptimized search function");
			std::size_t minIdx = 0;
			std::size_t maxIdx = accessor.size() - 1;

			if (argument == accessor.argument(0)) {
				maxIdx = 0;
			}
			else if (argument == accessor.argument(maxIdx)) {
				minIdx = maxIdx;
			}
			else {
				while (maxIdx > minIdx) {
					const auto midIdx = (maxIdx + minIdx) >> 1;
					const auto midArgument = accessor.argument(midIdx);

					if (midArgument < argument) {
						minIdx = midIdx + 1;
					}
					else if (midArgument > argument) {
						maxIdx = midIdx - 1;
					}
					else {
						maxIdx = minIdx = midIdx;
						break;
					}
				}
			}

			return Range(minIdx, maxIdx);
		}

		//! \tparam ArgumentType Typ argumentu o kt�ry pytamy
		//! \tparam ValueType Typ warto�ci
		template<typename ValueType, typename ArgumentType>
		//! \param accessor Kana� dyskretny w kt�rym szukamy otaczaj�cych pr�bek
		//! \param argument Argument wok� kt�rego szukamy pr�bek
		//! \return Zakres pr�bek otaczaj�cych dany argument (r�nica warto�ci indeks�w 1),
		//! kiedy pr�bka znajduje si� w kanale zakres o dw�ch identycznych indeksach (r�nica 0),
		//! kiedy pr�bka poza zakresem kana�u dowolny zakres indeks�w z r�nic� wieksz� ni� 1
		static inline Range range(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & argument, const ArgumentType & interval)
		{
			const std::size_t idx = (argument - accessor.argument(0)) / interval;
			if (accessor.argument(idx) == argument) {
				return Range(idx, idx);
			}

			return Range(idx, idx + 1);
		}
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__NEARESTARGUMENTSFINDER_H__