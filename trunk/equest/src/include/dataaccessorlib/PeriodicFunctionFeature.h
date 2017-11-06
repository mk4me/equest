/********************************************************************
	created:  2015/07/28	15:05:18
	filename: PeriodicFunctionFeature.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__PERIODICFUNCTIONFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__PERIODICFUNCTIONFEATURE_H__

#include <dataaccessorlib/Accessors.h>

namespace dataaccessor
{
	//! \rparam ArgumentType Typ argumentu kana�u
	template<class ArgumentType>
	//! Interfejs kana�u z ograniczonymi warto�ciami
	class PeriodicFunctionFeature : public FeatureHelperT<Periodic, IArgumentFeatureT<ArgumentType>>
	{
	private:

			const ArgumentType period_;

	public:

		template<typename T>
		PeriodicFunctionFeature(T && period)
			: period_(std::forward<T>(period)) {}

		//! Destruktor wirtualny
		~PeriodicFunctionFeature() {}

		//! \return D�ugo�� okresu kana�u
		ArgumentType period() const { return period_; }

		template<typename ValueType>
		inline static PeriodicFunctionFeature * create(
			const IDiscreteAccessor<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			return nullptr;
		}
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__PERIODICFUNCTIONFEATURE_H__
