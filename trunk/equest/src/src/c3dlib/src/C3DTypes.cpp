#include <c3dlib/C3DTypes.h>
#include <stdexcept>
#include <utils/Debug.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>
#include <dataaccessorlib/DescriptorFeature.h>
#include <dataaccessorlib/BoundedArgumentsFeature.h>

using namespace c3dlib;

EMGChannel::EMGChannel(const C3DParser& data, int channelNo) : C3DAnalogChannel(data.getAnalog(channelNo), 1.0 / data.getAnalogFrequency(), data.getNumAnalogFrames())
{

}

GRFChannel::GRFChannel(const C3DParser& data, int channelNo, float treshold) :
type(fromChannelNo(channelNo)), dataStart(-1.0f), dataEnd(-1.0f),
xsignal(data.getAnalog(3 * channelNo + 0)),
ysignal(data.getAnalog(3 * channelNo + 1)),
zsignal(data.getAnalog(3 * channelNo + 2)),
argumentsGenerator(1.0 / data.getAnalogFrequency(), data.getNumAnalogFrames())
{
	if (xsignal == nullptr || ysignal == nullptr || zsignal == nullptr) {
		throw std::runtime_error("Incomplete signal");
	}

	auto treshold2 = std::pow(treshold, 2);	

	bool dataStartSet = false;
	bool dataEndSet = false;

	int startIndex = -1;
	int endIndex = -1;

	const int numSamples = data.getNumAnalogFrames();
	for (int i = 0; i < numSamples; ++i) {
		osg::Vec3 val = osg::Vec3(xsignal->getValue(i), ysignal->getValue(i), zsignal->getValue(i));
		if (val.length2() > treshold2) {
			if (!dataStartSet) {
				dataStartSet = true;
				startIndex = i;
			}
		} else {
			if (dataStartSet && !dataEndSet) {
				endIndex = i;
				dataEndSet = true;
			}
		}
	}

	std::string name = xsignal->getLabel();
	auto it = std::remove(name.begin(), name.end(), 'x');
	if (it == name.end()) {
		it = std::remove(name.begin(), name.end(), 'X');
	}

	if (it != name.end()) {
		name.erase(it);
	}

	//attachFeature(utils::make_shared<dataaccessor::UniformArgumentsFeature<argument_type>>(argumentsGenerator.step()));
	attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<value_type, argument_type >(name, xsignal->getUnit(), "s")));
	//attachFeature(utils::make_shared<dataaccessor::BoundedArgumentsFeature<argument_type>>(argumentsGenerator.start(), argumentsGenerator.end()));

	if (dataStartSet) {
		dataStart = (startIndex / static_cast<float>(numSamples)) * argumentsGenerator.end();
	}
	if (dataEndSet) {
		dataEnd = (endIndex / static_cast<float>(numSamples)) * argumentsGenerator.end();
	}
}

GRFChannel::~GRFChannel()
{

}

GRFChannel::Type GRFChannel::fromChannelNo(const int channelNo)
{
	auto type = Unknown;

	switch (channelNo) {
	case 0:		type = F1;		break;
	case 1:		type = M1;		break;
	case 2:		type = F2;		break;
	case 3:		type = M2;		break;
	}

	return type;
}

GRFChannel::argument_type GRFChannel::argument(const size_type idx) const
{
	return argumentsGenerator.argument(idx);
}

GRFChannel::value_type GRFChannel::value(const size_type idx) const
{
	return value_type(xsignal->getValue(idx), ysignal->getValue(idx), zsignal->getValue(idx));
}

GRFChannel::size_type GRFChannel::size() const
{
	return argumentsGenerator.size();
}

GRFChannel::Type GRFChannel::getType() const
{
	return type;
}

bool GRFChannel::hasStartEndData() const
{
	return dataStart >= 0.0f && dataEnd >= 0.0f;
}

float GRFChannel::getDataStartTime() const
{
	UTILS_ASSERT(dataStart >= 0.0f); return dataStart;
}

float GRFChannel::getDataEndTime() const
{
	UTILS_ASSERT(dataEnd >= 0.0f); return dataEnd;
}