/********************************************************************
	created:	2011/12/11
	created:	11:12:2011   19:39
	filename: 	EMGFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EMGFILTER_H__
#define HEADER_GUARD_HMM__EMGFILTER_H__

//#include "TreeItemHelper.h"

#include <plugins/c3d/C3DChannels.h>
#include <plugins/newChart/INewChartSerie.h>
#include <plugins/hmmlib/Export.h>
#include <loglib/Exceptions.h>
#include <dataaccessorlib/BoundedValuesFeature.h>
#include <dataaccessorlib/Adapters.h>
#include <dataaccessorlib/Wrappers.h>

using namespace core;


struct RMSChannel
{
	template<typename ValueType, typename ArgumentType>
	using Accessor = dataaccessor::IDiscreteAccessorT<ValueType, ArgumentType>;

	template<typename ValueType, typename ArgumentType>
	static auto	create(const Accessor<ValueType, ArgumentType> & accessor, const std::size_t meanR = 250)
		-> decltype(dataaccessor::wrap(std::declval<std::vector<typename Accessor<ValueType, ArgumentType>::sample_type>>()))
	{
		using AccessorT = Accessor<ValueType, ArgumentType>;
		std::vector<typename AccessorT::sample_type> values;

		const auto count = accessor.size() - meanR;
		// brzeg lewy...
		for (std::size_t l_idx = 0; l_idx < meanR; ++l_idx) {
			ValueType sum = 0;
			for (std::size_t i = 0; i < l_idx + meanR; ++i) {
				sum += std::pow(accessor.value(i), 2.0);
			}
			sum /= (l_idx + meanR);
			sum = std::sqrt(sum);
			values.push_back({accessor.argument(l_idx), sum});
		}

		const auto d = 2 *meanR + 1;

		// właściwa interpolacja
		for (std::size_t idx = meanR; idx < count; ++idx) {
			ValueType sum = 0;
			for (std::size_t i = idx - meanR; i < idx + meanR + 1; ++i) {
				sum += std::pow(accessor.value(i), 2.0);
			}
			sum /= d;
			sum = std::sqrt(sum);
			values.push_back({ accessor.argument(idx), sum });
		}
		// brzeg prawy...
		for (std::size_t r_idx = count; r_idx < accessor.size(); ++r_idx) {
			ValueType sum = 0;
			for (std::size_t i = r_idx; i < accessor.size(); ++i) {
				sum += std::pow(accessor.value(i), 2.0);
			}
			sum /= accessor.size() - r_idx;
			sum = std::sqrt(sum);
			values.push_back({ accessor.argument(r_idx), sum });
		}

		return dataaccessor::wrap(std::move(values));
	}
};

class ScalarChannelIntegrator
{
public:

 //   pointT mean(const pointT& p1, const pointT& p2, const pointT& p3) const
 //   {
 //       return (p1 + p2 + p3) / 3;
 //   }
	//void operator()(c3dlib::ScalarChannelReaderInterface::_MyExtendedWriter & modifierInterface,
	//				const c3dlib::ScalarChannelReaderInterface::_MyRawChannelReaderType & observedChannel,
	//				const c3dlib::ScalarChannelReaderInterface::_MyRawChannelReaderType & myChannel)
 //   {
 //       ////uzupełnij brakujące próbki : co dziesiata...
 //       sizeT skipN = 10;
 //       if(myChannel.size() * skipN < observedChannel.size()){
 //           for(auto idx = myChannel.size() * skipN; idx < observedChannel.size(); idx += skipN){
 //               pointT sum = 0;
 //               sizeT right = idx + skipN;
 //               right = right < observedChannel.size() ? right : observedChannel.size();
 //               for (sizeT idx2 = idx; idx2 < right; ++idx2) {
 //                   sum += observedChannel.value(idx2);
 //               }
 //               modifierInterface.addPoint(observedChannel.argument(idx), sum / skipN);
 //           }
 //       }
 //       //aktualizacja próbek

 //       sizeT meanR = 10;
 //       sizeT count = myChannel.size() - meanR;

 //       for (sizeT step = 0; step < 50; ++step) {
 //           // brzeg lewy...
 //           for (sizeT l_idx = 0; l_idx < meanR; ++l_idx) {
 //               pointT sum = 0;
 //               for (sizeT i = 0; i < l_idx + meanR; ++i) {
 //                   sum += observedChannel.value(i);
 //               }
 //               sum /= (l_idx + meanR);
 //               modifierInterface.setIndexData(l_idx, sum);
 //           }
 //           // właściwa interpolacja
 //           for(sizeT idx = meanR; idx < count; ++idx) {
 //               pointT sum = 0;
 //               for (sizeT i = idx - meanR; i < idx + meanR + 1; ++i) {
 //                   sum += observedChannel.value(i);
 //               }
 //               sum /= (meanR + meanR + 1);
 //               modifierInterface.setIndexData(idx, sum);
 //           }
 //           // brzeg prawy...
 //           for (sizeT r_idx = count; r_idx < myChannel.size(); ++r_idx) {
 //               pointT sum = 0;
 //               for (sizeT i = r_idx; i < myChannel.size(); ++i) {
 //                   sum += observedChannel.value(i);
 //               }
 //               sum /= (r_idx - count + meanR);
 //               modifierInterface.setIndexData(r_idx, sum);
 //           }
 //       }

 //   }
};

template<class ValueType>
class AbsMeanValueExtractorT
{
public:

	AbsMeanValueExtractorT(const ValueType & mean)
		: mean_(mean)
    {

    }

	AbsMeanValueExtractorT(ValueType && mean)
		: mean_(std::move(mean))
	{

	}

	AbsMeanValueExtractorT(const AbsMeanValueExtractorT & Other) : mean_(Other.mean_)
	{

	}

	AbsMeanValueExtractorT(AbsMeanValueExtractorT && Other) : mean_(std::move(Other.mean_))
	{
		//Other.mean_ = ValueType();
	}

	~AbsMeanValueExtractorT() {}	

	inline ValueType extract(const ValueType & value, ...) const
    {
        return std::abs(value - mean_);
    }

private:

	const ValueType mean_;

};

struct AbsMeanValueChannel
{
	template<typename ValueType>
	inline static ValueType mean(const dataaccessor::IDiscreteValueAccessorT<ValueType> & accessor)
	{
		auto bvf = accessor.template getOrCreateFeature<dataaccessor::BoundedValuesFeature>();
		const ValueType min4 = bvf->minValue() / 4;
		const ValueType max4 = bvf->maxValue() / 4;

		ValueType sum = 0;
		//średnia
		for (std::size_t idx = 0; idx < accessor.size(); ++idx) {
			auto val = accessor.value(idx);
			if (val > max4) {
				val = max4;
			}
			else if (val < min4) {
				val = min4;
			}
			sum += val;
		}

		return sum / static_cast<ValueType>(accessor.size());
	}

	template<typename AccessorType>
	static inline dataaccessor::DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type> wrap(const typename AccessorType::value_type & mean, utils::shared_ptr<const AccessorType> accessor)
	{
		return dataaccessor::DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type>(new dataaccessor::SafeDiscreteAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type, AbsMeanValueExtractorT<typename AccessorType::value_type>>(accessor,
			AbsMeanValueExtractorT<typename AccessorType::value_type>(mean)));
	}

	template<typename AccessorType>
	static inline dataaccessor::DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type> wrap(utils::shared_ptr<const AccessorType> accessor)
	{
		return dataaccessor::DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type>(new dataaccessor::SafeDiscreteAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type, AbsMeanValueExtractorT<typename AccessorType::value_type>>(accessor,
			AbsMeanValueExtractorT<typename AccessorType::value_type>(AbsMeanValueChannel::mean(*accessor))));
	}

	template<typename ValueType, typename ArgumentType>
	static dataaccessor::DiscreteAccessorPtr<ValueType, ArgumentType> lightWrap(dataaccessor::IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
	{
		return dataaccessor::DiscreteAccessorPtr<ValueType, ArgumentType>(new dataaccessor::DiscreteAccessorAdapter<ValueType, ArgumentType, AbsMeanValueExtractorT<ValueType>>(*accessor,
			AbsMeanValueExtractorT<ValueType>(AbsMeanValueChannel::mean(accessor))));
	}

	template<typename ValueType, typename ArgumentType>
	static dataaccessor::DiscreteAccessorPtr<ValueType, ArgumentType> lightWrap(const ValueType & mean, dataaccessor::IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
	{
		return dataaccessor::DiscreteAccessorPtr<ValueType, ArgumentType>(new dataaccessor::DiscreteAccessorAdapter<ValueType, ArgumentType, AbsMeanValueExtractorT<ValueType>>(*accessor,
			AbsMeanValueExtractorT<ValueType>(mean)));
	}
};

#endif
