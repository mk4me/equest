/********************************************************************
	created:	2015/03/20
	created:	8:49
	filename: 	C3DTypes.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DTypes_H__
#define HEADER_GUARD_C3DLIB__C3DTypes_H__

#include <osg/Vec3>
#include <dataaccessorlib/ArgumentTracker.h>
#include <dataaccessorlib/Adapters.h>
#include <dataaccessorlib/AccessorsCollection.h>
#include <dataaccessorlib/AccessorsCollection.h>
#include <utils/SmartPtr.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <utils/Utils.h>
#include <c3dlib/C3DParser.h>
#include <c3dlib/Export.h>

namespace  c3dlib {

	using TimeType = float;
	using ScalarType = float;

	//! interfejs do odczytu czasu timera
	using TimerReader = dataaccessor::IArgumentTrackerReader<TimeType>;
	//! wskaźnik do interfejsu do odczytu czasu timera
	using TimerReaderPtr = utils::shared_ptr<TimerReader>;
	//! niemodyfikowalny wskaźnik do interfejsu do odczytu czasu timera
	using TimerReaderConstPtr = utils::shared_ptr<const TimerReader>;

	//! akcesor umożliwiajacy dostanie się do danych w sposób ciągły
	using ScalarContiniousTimeAccessor = dataaccessor::IFunctionAccessorT<ScalarType, TimeType>;
	
	//! interfejs umożliwiający odczyt kanału
	using ScalarChannelReaderInterface = dataaccessor::IDiscreteAccessorT<ScalarType, TimeType>;
	//! wkaźnik na interfejs umożliwiający odczyt kanału
	using ScalarChannelReaderInterfacePtr = utils::shared_ptr<ScalarChannelReaderInterface>;
	//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
	using ScalarChannelReaderInterfaceConstPtr = utils::shared_ptr<const ScalarChannelReaderInterface>;
	
	//! interfejs umożliwiający odczyt kanału
	using VectorChannelReaderInterface = dataaccessor::IDiscreteAccessorT<osg::Vec3, TimeType>;
	//! wkaźnik na interfejs umożliwiający odczyt kanału
	using VectorChannelReaderInterfacePtr = utils::shared_ptr<VectorChannelReaderInterface>;
	//! wkaźnik na niemodyfikowalny interfejs umożliwiający odczyt kanału
	using VectorChannelReaderInterfaceConstPtr = utils::shared_ptr<const VectorChannelReaderInterface>;

	template<typename SignalType, typename ValueType = typename utils::remove_toplevel<decltype(std::declval<SignalType>()->getValue(0))>::type>
	class C3DChannelWrapper : public dataaccessor::IIndependentDiscreteAccessorT<ValueType, TimeType>
	{
	public:

		using argument_type = typename dataaccessor::IIndependentDiscreteAccessorT<ValueType, TimeType>::argument_type;
		using size_type = typename dataaccessor::IIndependentDiscreteAccessorT<ValueType, TimeType>::size_type;
		using value_type = typename dataaccessor::IIndependentDiscreteAccessorT<ValueType, TimeType>::value_type;

	public:

		C3DChannelWrapper(const SignalType& signal, const argument_type step, const std::size_t size)
			: signal(signal),
			argumentsGenerator(step, size) {

			if (signal == nullptr) {
				throw std::runtime_error("No points signal");
			}

			//attachFeature(utils::make_shared<dataaccessor::UniformArgumentsFeature<argument_type>>(argumentsGenerator.step()));
			this->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::template create<value_type, argument_type >(signal->getLabel(), signal->getUnit(), "s")));
			//attachFeature(utils::make_shared<dataaccessor::BoundedArgumentsFeature<argument_type>>(argumentsGenerator.start(), argumentsGenerator.end()));
		}

		virtual ~C3DChannelWrapper() {}

		virtual argument_type argument(const size_type idx) const override {
			return argumentsGenerator.argument(idx);
		}

		virtual value_type value(const size_type idx) const override {
			return signal->getValue(idx);
		}

		virtual size_type size() const override {
			return argumentsGenerator.size();
		}

	private:
		const SignalType signal;
		const dataaccessor::UniformArgumentsGenerator<argument_type> argumentsGenerator;
	};

	using AnalogType = ScalarType;

	using C3DAnalogChannel = utils::IntegralTagWrapper<C3DChannelWrapper<C3DParser::IAnalogConstPtr, AnalogType>, 0>;

	using C3DAnalogChannelPtr = utils::shared_ptr<C3DAnalogChannel>;
	using C3DAnalogChannelConstPtr = utils::shared_ptr<const C3DAnalogChannel>;

	//! kanał EMG
	class C3DLIB_EXPORT EMGChannel : public C3DAnalogChannel
	{
	public:		
		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		EMGChannel(const c3dlib::C3DParser& data, int channelNo);
		//! Destruktor wirtualny
		virtual ~EMGChannel() {}
	};
	using EMGChannelPtr = utils::shared_ptr<EMGChannel>;
	using EMGChannelConstPtr = utils::shared_ptr<const EMGChannel>;

	//! kanał GRF
	class C3DLIB_EXPORT GRFChannel : public dataaccessor::IIndependentDiscreteAccessorT<osg::Vec3, TimeType>
	{
	public:
		//! rodzaj kanału GRF
		enum Type { Unknown, F1, M1, F2, M2 };

	public:		
		//! konstruktor
		//! \param c3dData przeparsowane dane z pliku c3d
		//! \param channelNo numer kanału w pliku c3d
		//! \param treshold próg dla którego warto analizować dane z płyty GRF
		GRFChannel(const c3dlib::C3DParser& data, int channelNo, float treshold = 200.0f);
		//! Destruktor wirtualny
		virtual ~GRFChannel();
		//! \param channelNo Numer kanału
		//! \return Typ kanału/danych
		static Type fromChannelNo(const int channelNo);

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override;

		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual value_type value(const size_type idx) const override;

		//! \return Ilość próbek w kanale
		virtual size_type size() const override;
	
		//! \return rodzaj kanału GRF
		Type getType() const;
		//! \return czy kanał ma wykryty peak, związanych z naciskiem na płytę
		bool hasStartEndData() const;
		//! \return początek danych, związanych z naciskiem na płytę
		argument_type getDataStartTime() const;
		//! \return koniec danych, związanych z naciskiem na płyę
		argument_type getDataEndTime() const;

	private:
		//! Typ kanału
		const Type type;
		//! początek danych, związanych z naciskiem na płytę
		argument_type dataStart;
		//! koniec danych, związanych z naciskiem na płytę
		argument_type dataEnd;
		//! wewnętrzne kanały
		const c3dlib::C3DParser::IAnalogConstPtr xsignal;
		const c3dlib::C3DParser::IAnalogConstPtr ysignal;
		const c3dlib::C3DParser::IAnalogConstPtr zsignal;
		//! Generator argumentów
		const dataaccessor::UniformArgumentsGenerator<argument_type> argumentsGenerator;
	};
	using GRFChannelPtr = utils::shared_ptr<GRFChannel>;
	using GRFChannelConstPtr = utils::shared_ptr<const GRFChannel>;

#define DECLARE_CHANNEL(name, type)	\
	class name##Channel : public utils::IntegralTagWrapper<C3DChannelWrapper<C3DParser::IPointConstPtr>, type> { \
	public:	\
		name##Channel(const C3DParser& data, int channelNo)	\
			: utils::IntegralTagWrapper<C3DChannelWrapper<C3DParser::IPointConstPtr>, type>(data.getPoint(channelNo),	\
			1.0 / data.getPointFrequency(), data.getNumPointFrames()) {}	\
		virtual ~name##Channel() {} \
	}; \
	using name##ChannelPtr = utils::shared_ptr<name##Channel>;	\
	using name##ChannelConstPtr = utils::shared_ptr<const name##Channel>;

	DECLARE_CHANNEL(Marker, 1);
	DECLARE_CHANNEL(Force, 2);
	DECLARE_CHANNEL(Moment, 3);
	DECLARE_CHANNEL(Angle, 4);
	DECLARE_CHANNEL(Power, 5);
}

#endif // HEADER_GUARD 
