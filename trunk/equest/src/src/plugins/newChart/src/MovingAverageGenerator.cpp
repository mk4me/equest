#include "NewChartPCH.h"
#include "MovingAverageGenerator.h"
#include "dataaccessorlib/Adapters.h"
#include "dataaccessorlib/Wrappers.h"

MovingAverageGenerator::~MovingAverageGenerator()
{
}

std::string MovingAverageGenerator::getName() const
{
	return "Moving average";
}

INewChartCurveGenerator::Params MovingAverageGenerator::getDefaultParams() const
{
	Params p;
	p.push_back(Param("w", 5));
	return p;
}

std::pair<c3dlib::ScalarChannelReaderInterfacePtr, std::string> MovingAverageGenerator::generate(const c3dlib::ScalarChannelReaderInterfaceConstPtr& sc, const Params& params) const
{
	UTILS_ASSERT(!params.empty() && params[0].value.type() == QVariant::Int);
	std::string title;
	auto df = sc->feature<dataaccessor::DescriptorFeature>();
	if (df != nullptr) {
		title += df->name() + std::string(":");
	}
	return std::make_pair(createMovingAverage(sc, params[0].value.toInt()), title + std::string("mv_av.") + boost::lexical_cast<std::string>(params[0].value.toInt()));
}

c3dlib::ScalarChannelReaderInterfacePtr MovingAverageGenerator::createMovingAverage(c3dlib::ScalarChannelReaderInterfaceConstPtr origin, int sampleWindow) const
{
	auto size = origin->size();
	if (size < sampleWindow) {
		return c3dlib::ScalarChannelReaderInterfacePtr();
	}

	auto uaf = origin->feature<dataaccessor::UniformArgumentsFeature<float>>();
	auto baf = origin->feature<dataaccessor::BoundedArgumentsFeature<float>>();
	auto ff = dataaccessor::FunctionFeature::feature(true);

	utils::shared_ptr<dataaccessor::DescriptorFeature> df(dataaccessor::DescriptorFeature::create<float, float>("name", "", ""));


	std::vector<float> vals;
	float startSum = 0.0f;
	for (auto i = 0; i < sampleWindow; ++i) {
		startSum += origin->value(i);
		vals.push_back(startSum / (i + 1));
	}
	vals.push_back(startSum / sampleWindow);
	for (auto i = sampleWindow; i < size; ++i) {
		startSum -= origin->value(i - sampleWindow);
		startSum += origin->value(i);
		vals.push_back(startSum / sampleWindow);
	}

	auto channel = dataaccessor::wrap(std::move(vals), dataaccessor::UniformArgumentsGenerator<float>(uaf->argumentsInterval(), vals.size()));
	channel->attachFeature(uaf);
	channel->attachFeature(baf);
	channel->attachFeature(ff);
	channel->attachFeature(df);
	return channel;
}