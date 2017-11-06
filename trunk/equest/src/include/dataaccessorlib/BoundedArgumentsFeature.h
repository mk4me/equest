/********************************************************************
created:  2015/07/28	15:04:47
filename: BoundedArgumentsFeature.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__BOUNDEDARGUMENTSFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__BOUNDEDARGUMENTSFEATURE_H__

#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/FunctionFeature.h>

namespace dataaccessor
{
	//! \tparam ArgumentType Typ argumentu kana�u danych
	template<typename ArgumentType>
	//! Interfesj opisuj�cy zakres argument�w
	class BoundedArgumentsFeature : public FeatureHelperT<ArgumentBounded, IArgumentFeatureT<ArgumentType>>
	{
	private:

			const ArgumentType min_;
			const ArgumentType max_;

	public:

		template<typename T, typename U>
		BoundedArgumentsFeature(T && min, U && max)
			: min_(std::forward<T>(min)), max_(std::forward<U>(max))
		{}

		//! Destruktor wirtualny
		~BoundedArgumentsFeature() {}
		//! \return Minimalna warto�� argumentu
		ArgumentType minArgument() const { return min_; }
		//! \return Maksymalna warto�c argumentu
		ArgumentType maxArgument() const { return max_; }

		static inline BoundedArgumentsFeature * create(
			const IDiscreteArgumentAccessorT<ArgumentType> & accessor)
		{
			BoundedArgumentsFeature * ret = nullptr;
			if (accessor.empty() == false) {

				auto feature = accessor.template feature<FunctionFeature>();

				auto min = accessor.argument(0);
				auto max = min;

				if (feature != nullptr && feature->isFunction() == true) {
					max = accessor.argument(accessor.size() - 1);
					auto r = std::minmax(min, max);
					min = r.first;
					max = r.second;
				}
				else {

					for (decltype(accessor.size()) idx = 1; idx < accessor.size(); ++idx)
					{
						const auto a = accessor.argument(idx);
						if (a < min) {
							min = a;
						}
						else if (a > max) {
							max = a;
						}
					}
				}

				ret = new BoundedArgumentsFeature(min, max);
			}

			return ret;
		}

		/*template<typename ValueType>
		static inline BoundedArgumentsFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			BoundedArgumentsFeature * ret = nullptr;
			if (discrete != nullptr) {
				discrete->template getOrCreateFeature<FunctionFeature>();
				ret = create(*discrete);
			}

			return ret;
		}*/
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__BOUNDEDARGUMENTSFEATURE_H__
