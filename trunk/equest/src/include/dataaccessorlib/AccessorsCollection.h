/********************************************************************
	created:	2011/07/04
	created:	4:7:2011   20:56
	filename: 	AccessorsCollection.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_UTILS__ACCESSORSCOLLECTION_H__
#define HEADER_GUARD_UTILS__ACCESSORSCOLLECTION_H__

#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/Adapters.h>
#include <dataaccessorlib/Traits.h>
#include <dataaccessorlib/DescriptorFeature.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utils/SmartPtr.h>
#include <utils/Debug.h>
#include <boost/tuple/tuple.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace dataaccessor {
	////////////////////////////////////////////////////////////////////////////////

	//! Klasa agreguje obiekty specyficznych akcesorów dyskretnych (funkcje), wszystkie dodawane akcesory powinny mieć tyle samo próbek
	template <typename AccessorType, ENABLE_IF(is_valid_discrete_accessor<AccessorType>::value)>
	class AccessorsCollection
	{
	public:
		using value_type = typename AccessorType::value_type;
		using argument_type = typename AccessorType::argument_type;
		using Accessor = AccessorType;
		using FAccessor = IFunctionAccessorT<value_type, argument_type>;
		using AccessorPtr = utils::shared_ptr<Accessor>;
		using AccessorConstPtr = utils::shared_ptr<const Accessor>;
		using FAccessorPtr = utils::shared_ptr<FAccessor>;
		using FAccessorConstPtr = utils::shared_ptr<const FAccessor>;

		template<typename Interpolator, typename Extrapolator>
		using FAdapter = DiscreteFunctionAccessorAdapter<value_type, argument_type, Interpolator, Extrapolator>;

		using Collection = std::vector<AccessorPtr>;
		using FCollection = std::vector<FAccessorPtr>;
		using iterator = typename Collection::iterator;
		using const_iterator = typename Collection::const_iterator;
		using ValueArgumentIndex = boost::tuple<value_type, argument_type, std::size_t>;
	protected:
		Collection accessors;
		FCollection faccessors;
		//! numer konfiguracji
		int configurationID;

	public:

		AccessorsCollection() : configurationID(-1) {}
		AccessorsCollection(const AccessorsCollection& dc)
			: accessors(dc.accessors), faccessors(dc.faccessors),
			configurationID(dc.configurationID)
		{			
		}

		AccessorsCollection(AccessorsCollection && dc)
			: accessors(std::move(dc.accessors)), faccessors(std::move(dc.faccessors)),
			configurationID(dc.configurationID)
		{
			dc.configurationID = -1;
		}
		
		virtual ~AccessorsCollection() {}

	public:
		template<typename Interpolator = LerpInterpolator,
			typename Extrapolator = BorderExtrapolator<value_type>>
		void addAccessor(const AccessorPtr & ptr, Interpolator && i = Interpolator(),
			Extrapolator && e = Extrapolator())
		{
			UTILS_ASSERT(accessors.size() == 0 || ptr->size() == accessors[0]->size());
			accessors.push_back(ptr);
			faccessors.push_back(FAccessorPtr(new FAdapter<Interpolator, Extrapolator>(*ptr,
				std::forward<Interpolator>(i), std::forward<Extrapolator>(e))));
		}

		const AccessorPtr & getAccessor(int index)
		{
			UTILS_ASSERT(index >= 0 && index < static_cast<int>(accessors.size()));
			return accessors[index];
		}

		AccessorConstPtr getAccessor(int index) const
		{
			UTILS_ASSERT(index >= 0 && index < static_cast<int>(accessors.size()));
			return accessors[index];
		}

		AccessorConstPtr getAccessor(const std::string& name) const
		{
			for (auto & a : accessors)
			{
				auto f = a->template feature<DescriptorFeature>();
				if (f != nullptr && f->name() == name){
					return a;
				}
			}

			throw std::runtime_error("Unknown accessor name");
		}

		int getNumPointsPerAccessor() const
		{
			UTILS_ASSERT(accessors.size());
			return accessors[0]->size();
		}

		int getNumAccessors() const
		{
			return accessors.size();
		}

		//! \return długość kanału w sekundach; gdy nie ma danych zwracane jest zero, jest to odejście od konwecji, ale dzialanie całkiem naturalne.
		argument_type getLength() const
		{
			if (accessors.size() == 0) {
				return argument_type(0);
			}
			else {
				return accessors[0]->argument(getNumPointsPerAccessor() - 1);
			}
		}

		//! \param time czas, dla którego pobieramy wartości
		//! \return wartości wszystkich kanałów dla konkretnego czasu
		std::vector<value_type> getValues(const argument_type & argument) const
		{
			std::vector<value_type> ret;
			ret.reserve(getNumAccessors());
			for (auto & fa : faccessors)
			{
				ret.push_back(fa->value(argument));
			}

			return ret;
		}

		int getConfigurationID() const
		{
			return configurationID;
		}

		void setConfigurationID(int id)
		{
			configurationID = id;
		}

		//! \param index indeks kanału, dla którego pobieramy wartość
		//! \param time czas, dla którego będzie zwrócona wartość
		//! \return wartość kanału
		value_type getValue(int index, const argument_type & argument) const
		{
			return faccessors[index]->value(argument);
		}

		//! \return krotka z maksymalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
		template<class _Fun>
		ValueArgumentIndex getValue(_Fun f) const
		{
			std::vector<ValueArgumentIndex> values;
			
			for (int channelIdx = 0; channelIdx < accessors.size(); ++channelIdx) {
				auto chosenVal = accessors[channelIdx]->value(0);
				int chosenIdx = 0;
				int count = accessors[channelIdx]->size();
				for (int i = 1; i < count; ++i) {
					auto val = accessors[channelIdx]->value(i);
					if (f(chosenVal, val)) {
						chosenVal = val;
						chosenIdx = i;
					}
				}
				values.push_back(boost::make_tuple(
					accessors[channelIdx]->value(chosenIdx),
					accessors[channelIdx]->argument(chosenIdx),
					channelIdx));
			}

			const auto count = values.size();
			auto chosenVal = values[0].template get<0>();
			int chosenIdx = 0;
			for (int i = 1; i < count; ++i) {
				auto val = values[i].template get<0>();
				if (f(chosenVal, val)) {
					chosenIdx = i;
					chosenVal = val;
				}
			}

			return values[chosenIdx];
		}

		//! \return krotka z minimalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
		ValueArgumentIndex getMinValue() const
		{
			return getValue([&](const value_type& p1, const value_type& p2) { return p2 < p1; });
		}

		//! \return krotka z maksymalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
		ValueArgumentIndex getMaxValue() const
		{
			return getValue([&](const value_type& p1, const value_type& p2) { return p1 < p2; });
		}
	};
	////////////////////////////////////////////////////////////////////////////////
} //namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif
