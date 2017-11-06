/********************************************************************
created:  2015/07/28	15:04:12
filename: FunctionFeature.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__FUNCTIONFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__FUNCTIONFEATURE_H__

#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>
#include <set>
#include <utils/Utils.h>
#include <type_traits>
#include <utils/Export.h>

#if defined (dataaccessorlib_EXPORTS)
#define DATAACCESSORLIB_EXPORT UTILS_DECL_EXPORT
#else
#define DATAACCESSORLIB_EXPORT UTILS_DECL_IMPORT
#endif

namespace dataaccessor
{
	//! Interfejs opisuj�cy cechy kana�y reprezentuj�cego funkcj� - jeden argument jedna warto��,
	//! jak kana� nie ma tego interfejsu to traktujemy go jako relacje i mo�liwa jest tylko wersja dyskretna
	//! - musi j� implementowa�, inaczej FAIL
	class DATAACCESSORLIB_EXPORT FunctionFeature : public FeatureHelperT<Function>
	{
	private:
			static const utils::shared_ptr<FunctionFeature> functionFeature;
			static const utils::shared_ptr<FunctionFeature> nonFunctionFeature;

	public:

		FunctionFeature(const bool function) : function(function) {}

		//! Destruktor wirtualny
		~FunctionFeature() {}
		//! \return Czy funkcja jest parzysta
		bool isFunction() const { return function; }

		static utils::shared_ptr<FunctionFeature> feature(const bool isFunc);

		template<typename ValueType, typename ArgumentType>
		inline static utils::shared_ptr<FunctionFeature> create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			if (discrete != nullptr) {
				return create(*discrete);
			}
			return functionFeature;
		}

		template<typename ArgumentType>
		inline static utils::shared_ptr<FunctionFeature> create(
			const IDiscreteArgumentAccessorT<ArgumentType> & accessor)
		{
			bool isFun = true;

			if (accessor.empty() == true) {
				isFun = false;
			}
			else if ((accessor.size() > 1) && (accessor.template getOrCreateFeature<UniformArgumentsFeature>() == nullptr)) {
				auto sA = accessor.argument(0);
				auto sB = accessor.argument(1);
				if (sA != sB) {
					std::set<ArgumentType> arguments;
					arguments.insert(sA);
					arguments.insert(sB);
					const auto signum = utils::sign(sB - sA);
					auto locSignum = signum;
					for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
					{
						sA = sB;
						sB = accessor.argument(idx);
						locSignum = utils::sign(sB - sA);
						if (locSignum != signum) {
							isFun = false;
							break;
						}

						if (arguments.find(sB) == arguments.end()) {
							arguments.insert(signum > 0 ? arguments.end() : arguments.begin(), sB);
						}
						else {
							isFun = false;
							break;
						}						
					}					
				}
			}

			return feature(isFun);
		}

	private:

		const bool function;
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__FUNCTIONFEATURE_H__
