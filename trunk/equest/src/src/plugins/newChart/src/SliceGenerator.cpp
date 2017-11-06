#include "NewChartPCH.h"
#include "SliceGenerator.h"
#include "dataaccessorlib/Wrappers.h"

c3dlib::ScalarChannelReaderInterfacePtr SliceGenerator::generator(const c3dlib::ScalarChannelReaderInterfaceConstPtr& sc, double x1, double x2)
{
	auto count = sc->size();

	auto uaf = sc->feature<dataaccessor::UniformArgumentsFeature<float>>();
	auto baf = sc->feature<dataaccessor::BoundedArgumentsFeature<float>>();
	auto ff = dataaccessor::FunctionFeature::feature(true);

	utils::shared_ptr<dataaccessor::DescriptorFeature> df(dataaccessor::DescriptorFeature::create<float, float>("name", "", ""));


	std::vector<c3dlib::ScalarChannelReaderInterface::sample_type> data;

	for (auto i = 0; i < count; ++i) {
		auto arg = sc->argument(i);
		if (arg >= x1 && arg <= x2) {
			auto val = sc->value(i);
			data.push_back({ arg, val });
		}
	}

	auto channel = dataaccessor::wrap(std::move(data));

	channel->attachFeature(uaf);
	channel->attachFeature(baf);
	channel->attachFeature(ff);
	channel->attachFeature(df);
	return channel;
}
