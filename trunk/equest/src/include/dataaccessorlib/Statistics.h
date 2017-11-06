/********************************************************************
created:	2011/12/10
created:	10:12:2011   19:28
filename: 	Statistics.h
author:		Wojciech Kniec

purpose:
*********************************************************************/

#ifndef HEADER_GUARD_DATAACCESSOR__STATISTICS_H__
#define HEADER_GUARD_DATAACCESSOR__STATISTICS_H__

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/mpl/always.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <type_traits>
#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/FunctionFeature.h>

// Implementacje dodatkowych akumulatorow
namespace boost {

	namespace accumulators
	{

		namespace impl
		{
			//! Implementacja akumulatora wyszukującego minimum oraz indeks, dla którego minimum wystąpiło
			template <typename Sample>
			struct min_with_idex_impl
				: accumulator_base
			{
				//! zwracany typ - para : minimum, index
				using result_type = std::pair<Sample, std::size_t>;

				template<typename Args>
				min_with_idex_impl(Args const &args) :
					minimum(numeric::as_max(args[sample | Sample()])),
					count(0), index(0)
				{
				}

				//! wywoływane po dadanu kolejnej próbki
				template<typename Args>
				void operator ()(Args const &args)
				{					
					const auto & current = args[sample];
					if (current < minimum) {
						minimum = current;
						index = count;
					}
					++count;
				}


				//! Odpytanie o aktualny stan akumulatora
				//! \param  brak
				//! \return para zawierająca minimum i index dla niego
				result_type result(dont_care) const
				{
					return std::make_pair(this->minimum, this->index);
				}

			private:
				//! aktualne minimum dla podanych próbek
				Sample minimum;
				//! indeks, dla którego wystąpiło aktualne minimum
				std::size_t index;
				//! liczba podanych próbek (potrzebne by wyliczyć indeks)
				std::size_t count;
			};

			//! Implementacja akumulatora wyszukującego maksimum oraz indeks, dla którego maksimum wystąpiło
			//! implementacja analogiczna do min_with_index_impl
			template <typename Sample>
			struct max_with_idex_impl
				: accumulator_base
			{
				using result_type = std::pair<Sample, std::size_t>;

				template<typename Args>
				max_with_idex_impl(Args const &args) :
					maximum(numeric::as_min(args[sample | Sample()])),
					count(0), index(0)
				{
				}

				template<typename Args>
				void operator ()(Args const &args)
				{
					const auto & current = args[sample];
					if (current > maximum) {
						maximum = current;
						index = count;
					}
					++count;
				}

				result_type result(dont_care) const
				{
					return std::make_pair(this->maximum, this->index);
				}

			private:
				Sample maximum;
				std::size_t index;
				std::size_t count;
			};

		} // namespace impl

		// tagi, pozwalające umieszczać akumulatory w accumulator_set
		namespace tag
		{
			//! tag dla akumulatora minimum z indeksem
			struct min_with_index
				: depends_on<>
			{
				using impl = accumulators::impl::min_with_idex_impl<mpl::_1>;
			};

			//! tag dla akumulatora maksimum z indeksem
			struct max_with_index
				: depends_on<>
			{
				using impl = accumulators::impl::max_with_idex_impl<mpl::_1>;
			};
		}

		// ekstraktory, umozliwiaja odpytanie o statystyki
		namespace extract
		{
			extractor<tag::max_with_index> const max_with_index = {};
			extractor<tag::min_with_index> const min_with_index = {};

			BOOST_ACCUMULATORS_IGNORE_GLOBAL(max_with_index)
				BOOST_ACCUMULATORS_IGNORE_GLOBAL(min_with_index)
		}

		using extract::max_with_index;
		using extract::min_with_index;

	}
} // namespace boost::accumulators

////////////////////////////////////////////////////////////////////////////////
namespace dataaccessor {
	////////////////////////////////////////////////////////////////////////////////


//! Klasa ze statystykami kanału, leniwie aktualizowana w momencie zapytania
	class StatsHelper
	{
	private:

		using features = boost::accumulators::features< boost::accumulators::tag::mean, boost::accumulators::tag::median, boost::accumulators::tag::variance >;

		template<typename T>
		using Stats = boost::accumulators::accumulator_set<T, features>;

	public:

		template<typename T>
		struct Results
		{
			//! Średnia wartość danych w kanale
			T mean;
			//! Mediana
			T median;
			//! Wariancja danych w kanale
			T variance;
			//! Wariancja danych w kanale
			T standardDeviation;
		};

		template<typename ValueType, typename ArgumentType>
		struct CompleteResult
		{
			Results<ValueType> values;
			Results<ArgumentType> arguments;
		};

		template<typename ValueType, typename ArgumentType>
		static inline Results<ValueType> values(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const IDiscreteAccessor::size_type to,
			const IDiscreteAccessor::size_type from = 0)
		{
			using namespace boost::accumulators;			

			Results<ValueType> result;

			auto ff = accessor.template feature<FunctionDescriptionFeature>();

			if (ff != nullptr && ff->monotony() == Constant){
				result.mean = result.median = accessor.value(0);
				result.variance = result.standardDeviation = ValueType(0);				
			}
			else{
				Stats<ValueType> sts;

				for (auto i = from; i < to; ++i){
					sts(accessor.value(i));
				}

				result.mean = mean(sts);
				result.median = median(sts);
				result.variance = variance(sts);
				result.standardDeviation = std::sqrt(result.variance);
			}		

			return result;
		}

		template<typename ValueType, typename ArgumentType>
		static inline Results<ValueType> values(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return values(accessor, accessor.size());
		}

		template<typename ValueType, typename ArgumentType>
		static inline Results<ArgumentType> arguments(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const IDiscreteAccessor::size_type to,
			const IDiscreteAccessor::size_type from = 0)
		{
			using namespace boost::accumulators;		

			Results<ArgumentType> result;

			Stats<ArgumentType> sts;

			for (auto i = from; i < to; ++i){
				sts(accessor.argument(i));
			}

			result.mean = mean(sts);
			result.median = median(sts);
			result.variance = variance(sts);
			result.standardDeviation = std::sqrt(result.variance);			

			return result;
		}

		template<typename ValueType, typename ArgumentType>
		static inline Results<ArgumentType>	arguments(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return arguments(accessor, accessor.size());
		}

		template<typename ValueType, typename ArgumentType>
		static inline CompleteResult<ValueType, ArgumentType>
			samples(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const IDiscreteAccessor::size_type to,
			const IDiscreteAccessor::size_type from = 0)
		{
			using namespace boost::accumulators;
			CompleteResult<ValueType, ArgumentType> result;

			Stats<ArgumentType> argSts;

			auto fdf = accessor.template feature<FunctionDescriptionFeature>();

			if (fdf != nullptr && fdf->monotony() == Constant){
				result.values.mean = accessor.value(0);
				result.values.variance = result.values.standardDeviation = ValueType(0);

				for (auto i = from; i < to; ++i){					
					argSts(accessor.argument(i));
				}			
			}
			else{

				Stats<ValueType> valSts;

				for (auto i = from; i < to; ++i){
					const auto s = accessor.sample(i);
					argSts(s.first);
					valSts(s.second);
				}

				result.values.mean = mean(valSts);
				result.values.median = median(valSts);
				result.values.variance = variance(valSts);
				result.values.standardDeviation = std::sqrt(result.values.variance);
			}

			result.arguments.mean = mean(argSts);			
			result.arguments.median = median(argSts);
			result.arguments.variance = variance(argSts);
			result.arguments.standardDeviation = std::sqrt(result.arguments.variance);

			return result;
		}

		template<typename ValueType, typename ArgumentType>
		static inline CompleteResult<ValueType, ArgumentType>
			samples(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return samples(accessor, accessor.size());
		}
	};


	template<typename ValueType, typename ArgumentType>
	class StatisticsFeature : public FeatureHelperT<Statistics, IAccessorFeatureT<ValueType, ArgumentType>>
	{
	public:

		using Values = StatsHelper::Results<ValueType>;
		using Arguments = StatsHelper::Results<ArgumentType>;

	public:
		template<typename RV, typename RA>
		StatisticsFeature(RV && values,	RA && arguments)
			: values_(std::forward<RV>(values)),
			  arguments_(std::forward<RA>(arguments))
		{

		}

		//! Destruktor wirtualny
		~StatisticsFeature() {}
		//! \return Minimalna wartość argumentu
		Values values() const { return values_; }
		//! \return Maksymalna wartośc argumentu
		Arguments arguments() const { return arguments_; }

		static StatisticsFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			StatisticsFeature * ret = nullptr;
			if (discrete != nullptr && discrete->empty() == false){
				const auto stats = StatsHelper::samples(*discrete);
				ret = new StatisticsFeature(stats.values, stats.arguments);
			}

			return ret;
		}

	private:

		Values values_;
		Arguments arguments_;

	};

	////////////////////////////////////////////////////////////////////////////////
} //namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif
