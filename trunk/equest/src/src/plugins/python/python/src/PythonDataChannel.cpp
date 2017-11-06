#include <plugins/python/python/PythonDataChannel.h>
#include <dataaccessorlib/Wrappers.h>

namespace py = boost::python;

//python::PythonDataChannel::PythonDataChannel() :
//	frequency(0.0f)
//{
//
//}
//
//python::PythonDataChannel::~PythonDataChannel()
//{
//
//}
//
//python::PythonDataChannel python::PythonDataChannel::convert(c3dlib::VectorChannelReaderInterfaceConstPtr channel)
//{
//	UTILS_ASSERT(channel);
//	PythonDataChannel c;
//	auto count = channel->size();
//
//	for (auto i = 0; i < count; ++i) {
//		c.data.push_back(channel->value(i));
//	}
//
//	auto adf = channel->feature<dataaccessor::DescriptorFeature>();
//	if (adf != nullptr) {
//		c.name = adf->name();
//	}
//
//	auto uaf = channel->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
//	if (uaf != nullptr) {
//		c.frequency = 1.0 / uaf->argumentsInterval();
//	}
//	return c;
//}
//
//c3dlib::VectorChannelReaderInterfacePtr python::PythonDataChannel::convert(const PythonDataChannel& obj)
//{
//	auto name = obj.getName();
//
//	if (name.empty() == true) {
//		name = "Result";
//	}
//
//	auto wdata = obj.getData();
//	const auto size = wdata.size();
//	auto uaf = utils::make_shared<dataaccessor::UniformArgumentsFeature<c3dlib::VectorChannelReaderInterface::argument_type>>(1.0 / obj.getFrequency());
//	auto baf = utils::make_shared<dataaccessor::BoundedArgumentsFeature<c3dlib::VectorChannelReaderInterface::argument_type>>(0.0, (size - 1) / obj.getFrequency());
//	auto ff = dataaccessor::FunctionFeature::feature(true);
//
//	utils::shared_ptr<dataaccessor::DescriptorFeature> df(dataaccessor::DescriptorFeature::create<c3dlib::VectorChannelReaderInterface::value_type,
//		c3dlib::VectorChannelReaderInterface::argument_type>(name, "", ""));
//
//	auto channel = dataaccessor::wrap(std::move(wdata), dataaccessor::UniformArgumentsGenerator<c3dlib::VectorChannelReaderInterface::argument_type>(uaf->argumentsInterval(), size));
//	channel->attachFeature(uaf);
//	channel->attachFeature(baf);
//	channel->attachFeature(ff);
//	channel->attachFeature(df);
//	return channel;
//}
//
//std::vector<osg::Vec3> python::PythonDataChannel::getData() const
//{
//	return data;
//}
//
//float python::PythonDataChannel::getFrequency() const
//{
//	return frequency;
//}
//
//void python::PythonDataChannel::setFrequency(float frq)
//{
//	this->frequency = frq;
//}
//
//std::string python::PythonDataChannel::getName() const
//{
//	return name;
//}
//
//void python::PythonDataChannel::setData(const std::vector<osg::Vec3>& data)
//{
//	this->data = data;
//}
//
//void python::PythonDataChannel::setName(const std::string& name)
//{
//	this->name = name;
//}
//
//
