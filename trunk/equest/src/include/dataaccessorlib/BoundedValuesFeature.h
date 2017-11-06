/********************************************************************
created:  2015/07/28	15:05:37
filename: BoundedValuesFeature.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__BOUNDEDVALUESFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__BOUNDEDVALUESFEATURE_H__

#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/FunctionDescriptionFeature.h>

namespace dataaccessor
{
	//! \rparam ValueType Typ warto�ci kana�u
	template<class ValueType>
	//! Interfejs kana�u z ograniczonymi warto�ciami
	class BoundedValuesFeature : public FeatureHelperT<ValueBounded, IValueFeatureT<ValueType>>
	{
	private:

		const ValueType min_;
		const ValueType max_;

	public:

		template<typename T, typename U>
		BoundedValuesFeature(T && min, U && max)
			: min_(std::forward<T>(min)), max_(std::forward<U>(max))
		{}

		//! Destruktor wirtualny
		~BoundedValuesFeature() {}

		//! \return Dolna granica warto�ci
		ValueType minValue() const { return min_; }

		//! \return G�rna granica warto�ci
		ValueType maxValue() const { return max_; }

		//! Sprawdzamy czy wszystkie argumenty znajduj� si� w zadanym przedziale
		static inline BoundedValuesFeature * create(const IDiscreteValueAccessorT<ValueType> & accessor)
		{
			BoundedValuesFeature * ret = nullptr;
			if (accessor.empty() == false) {

				auto feature = accessor.template feature<FunctionDescriptionFeature>();

				auto min = accessor.value(0);
				auto max = min;

				if (feature != nullptr && feature->monotony() != NonMonotonic) {
					max = accessor.value(accessor.size() - 1);
					auto r = std::minmax(min, max);
					min = r.first;
					max = r.second;
				}
				else {

					for (decltype(accessor.size()) idx = 1; idx < accessor.size(); ++idx)
					{
						const auto v = accessor.value(idx);
						if (v < min) {
							min = v;
						}
						else if (v > max) {
							max = v;
						}
					}
				}

				ret = new BoundedValuesFeature(min, max);
			}

			return ret;
		}

		//! Sprawdzamy czy wszystkie argumenty znajduj� si� w zadanym przedziale
		/*template<typename ArgumentType>
		static inline BoundedValuesFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			BoundedValuesFeature * ret = nullptr;
			if (discrete != nullptr) {
				discrete->template getOrCreateFeature<FunctionFeature>();
				ret = create(*discrete);
			}

			return ret;
		}*/
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__BOUNDEDVALUESFEATURE_H__
