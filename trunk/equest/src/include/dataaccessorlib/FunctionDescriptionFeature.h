/********************************************************************
	created:  2016/02/05
	filename: FunctionDescriptionFeature.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__FUNCTIONDESCRIPTIONFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__FUNCTIONDESCRIPTIONFEATURE_H__

#include <dataaccessorlib/Accessors.h>
#include <set>
#include <utils/Utils.h>
#include <type_traits>

namespace dataaccessor
{
	//! Interfejs opisuj�cy cechy kana�y reprezentuj�cego funkcj� - jeden argument jedna warto��,
	//! jak kana� nie ma tego interfejsu to traktujemy go jako relacje i mo�liwa jest tylko wersja dyskretna
	//! - musi j� implementowa�, inaczej FAIL
	class FunctionDescriptionFeature : public FeatureHelperT<FunctionDescription>
	{
	public:

		FunctionDescriptionFeature(const bool even, const bool odd,
					const MonotonyType monotony)
					: even_(even), odd_(odd), monotony_(monotony)
				{}

		//! Destruktor wirtualny
		~FunctionDescriptionFeature() {}
		//! \return Czy funkcja jest parzysta
		bool even() const { return even_; }
		//! \return Czy funkcja jest nieparzysta
		bool odd() const { return odd_; }
		//! \return Czy funkcja jest monotoniczna
		MonotonyType monotony() const { return monotony_; }

		template<typename ValueType, typename ArgumentType>
		inline static FunctionDescriptionFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			FunctionDescriptionFeature * ret = nullptr;

			if (discrete != nullptr && discrete->template getOrCreateFeature<FunctionFeature>()->isFunction() == true) {
				ret = create(*discrete, std::is_arithmetic<ValueType>());
			}

			return ret;
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionDescriptionFeature * create(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			std::true_type)
		{
			using Discrete = IDiscreteAccessorT<ValueType, ArgumentType>;
			FunctionDescriptionFeature * ret = nullptr;
			bool even = false;
			bool odd = false;
			MonotonyType monotony = Constant;

			auto hasUF = (accessor.template getOrCreateFeature<UniformArgumentsFeature>() != nullptr);

			if (accessor.size() == 1) {
				const auto s = accessor.sample(0);
				even = (s.first == Discrete::argument_type());
				odd = (s.second == -s.second);
				ret = new FunctionDescriptionFeature(even, odd, monotony);
			}
			else {
				auto sA = accessor.sample(0);
				auto sB = accessor.sample(1);
				if (sA.first != sB.first) {

					const auto vmonotony = utils::sign(sB.second - sA.second);

					std::set<typename Discrete::argument_type> arguments;
					arguments.insert(sA.first);
					arguments.insert(sB.first);
					const auto signum = utils::sign(sB.first - sA.first);
					auto locSignum = signum;
					for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
					{
						sA = sB;
						sB = accessor.sample(idx);
						locSignum = utils::sign(sB.first - sA.first);
						if (locSignum != signum) {
							break;
						}

						if (arguments.find(sB.first) == arguments.end()) {
							arguments.insert(signum > 0 ? arguments.end() : arguments.begin(), sB.first);
						}
						else {
							--locSignum;
							break;
						}

						if (monotony != NonMonotonic) {
							auto locMonotony = utils::sign(sB.second - sA.second);
							auto diff = std::abs(locMonotony - vmonotony);
							if (diff > 1) {
								monotony = NonMonotonic;
								if (hasUF == true) {
									break;
								}
							}
							else if (diff == 1 && !((monotony == NonDecreasing) || (monotony == NonGrowing))) {
								if (monotony == Decreasing) {
									monotony = NonGrowing;
								}
								else if (monotony == Growing) {
									monotony = NonDecreasing;
								}
								else if (monotony == Constant) {
									if (locMonotony > 0) {
										monotony = Growing;
									}
									else {
										monotony = Decreasing;
									}
								}
							}
						}
					}

					if (locSignum == signum) {
						ret = new FunctionDescriptionFeature(even, odd, monotony);
					}
				}
			}

			return ret;
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionDescriptionFeature * create(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			std::false_type)
		{
			using Discrete = IDiscreteAccessorT<ValueType, ArgumentType>;
			FunctionDescriptionFeature * ret = nullptr;
			bool even = false;
			bool odd = false;
			MonotonyType monotony = NonMonotonic;

			if (accessor.size() == 1) {
				const auto s = accessor.sample(0);
				even = (s.first == Discrete::argument_type());
				odd = (s.second == -s.second);
				ret = new FunctionDescriptionFeature(even, odd, monotony);
			}
			else {
				auto sA = accessor.argument(0);
				auto sB = accessor.argument(1);
				bool ok = true;
				if (sA != sB) {
					std::set<typename Discrete::argument_type> arguments;
					arguments.insert(sA);
					arguments.insert(sB);
					for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
					{
						sA = sB;
						sB = accessor.argument(idx);

						if ((ok = arguments.insert(sB).second) == false) {
							break;
						}
					}

					if (ok == true) {
						ret = new FunctionDescriptionFeature(even, odd, monotony);
					}
				}
			}

			return ret;
		}

	private:
			const bool even_;
			const bool odd_;
			const MonotonyType monotony_;
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__FUNCTIONDESCRIPTIONFEATURE_H__
