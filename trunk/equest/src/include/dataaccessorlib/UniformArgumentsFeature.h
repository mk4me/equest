/********************************************************************
	created:  2015/07/28	15:02:06
	filename: UniformArgumentsFeature.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSFEATURE_H__

#include <limits>
#include <type_traits>
#include <dataaccessorlib/Accessors.h>

namespace dataaccessor
{
	//! \tparam ArgumentType Typ argumentu kana�u
	template<typename ArgumentType>
	//! Interfejs kana�u dyskretnego o r�wno oddalonych argumentach
	class UniformArgumentsFeature : public FeatureHelperT<UniformArguments, IArgumentFeatureT<ArgumentType>>
	{
	private:

			const ArgumentType interval;

	public:

		template<typename T>
		UniformArgumentsFeature(T && interval)
			: interval(std::forward<T>(interval))
		{}

		//! Destruktor wirtualny
		~UniformArgumentsFeature() {};

		//! \return Czas trwania kana�u
		ArgumentType argumentsInterval() const { return interval; }

		/*template<typename ValueType>
		static inline UniformArgumentsFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			UniformArgumentsFeature * ret = nullptr;
			if (discrete != nullptr) {
				ret = create(*discrete);
			}

			return ret;
		}*/
		
		static inline UniformArgumentsFeature * create(const IDiscreteArgumentAccessorT<ArgumentType> & accessor)
		{
			UniformArgumentsFeature * ret = nullptr;

			if (accessor.size() > 1){

				auto argA = accessor.argument(0);
				auto argB = accessor.argument(1);

				ArgumentType interval = (argB - argA);
				bool ok = true;

				for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
				{
					argA = argB;
					argB = accessor.argument(idx);

					if (interval != (argB - argA)){
						ok = false;
						break;
					}
				}
				
				if (ok == true){
					ret = new UniformArgumentsFeature(interval);
				}
			}

			return ret;
		}
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSFEATURE_H__
