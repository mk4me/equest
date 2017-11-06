#include <utils/SamplesStatus.h>
#include <numeric>

using namespace utils;

SamplesStatus::SamplesStatus(const unsigned int windowSize,
	const StatusMap & statusMap)
	: acc(boost::accumulators::tag::rolling_window::window_size = windowSize),
	statusMap(statusMap)
{
	if (windowSize <= 0){
		throw std::runtime_error("Invalid windows size - must be greater than 0");
	}

	if (statusMap.empty() == true)
	{
		throw std::runtime_error("Invalid status map - empty");
	}

	for (const auto & val : statusMap)
	{
		if (val.first < 0.0 || val.first > 1.0){
			throw std::runtime_error("Invalid status range value - [0, 1)");
		}
	}
}

SamplesStatus::SamplesStatus(const unsigned int windowSize,
	StatusMap && statusMap)
	: acc(boost::accumulators::tag::rolling_window::window_size = windowSize),
	statusMap(statusMap)
{
	if (windowSize <= 0){
		throw std::runtime_error("Invalid windows size - must be greater than 0");
	}

	if (statusMap.empty() == true)
	{
		throw std::runtime_error("Invalid status map - empty");
	}

	for (const auto & val : statusMap)
	{
		if (val.first < 0.0 || val.first > 1.0){
			throw std::runtime_error("Invalid status range value - [0, 1)");
		}
	}
}

SamplesStatus::~SamplesStatus()
{

}

void SamplesStatus::positiveSample()
{
	sample(Positive);
}

void SamplesStatus::negativeSample()
{
	sample(Negative);
}

void SamplesStatus::unspecifiedSample()
{
	sample(Unspecified);
}

void SamplesStatus::sample(const SampleType val)
{
	float s = 0.5;
	switch (val)
	{
	case Positive:
		s = 1.0;
		break;

	case Negative:
		s = 0.0;
		break;
	}

	acc(s);
}

int SamplesStatus::status() const
{
	const auto val = boost::accumulators::rolling_mean(acc);
	auto it = statusMap.lower_bound(val);
	if (it != statusMap.end()){
		return it->second;
	}
	else {
		return statusMap.rbegin()->second;
	}	
}

SamplesStatus SamplesStatus::createBinaryStatus(const int statusLower, const int statusHigher,
	const float splitValue, const unsigned int windowSize)
{
	if (splitValue < 0.0 || splitValue >= 1.0){
		throw std::runtime_error("Invalid splitValue for status range - [0, 1)");
	}

	StatusMap statusMap;
	statusMap.insert({ splitValue, statusLower });
	statusMap.insert({ 1.0f, statusHigher });

	return SamplesStatus(windowSize, statusMap);
}

SamplesStatus SamplesStatus::createBinaryStatus(const int statusLower, const int statusHigher,
	const int minimumPositiveSamples, const unsigned int windowSize)
{
	if (minimumPositiveSamples < 0 || minimumPositiveSamples >= windowSize){
		throw std::runtime_error("Invalid minimumPositiveSamples for status range - [0, windowSize)");
	}

	const float splitValue = (float)minimumPositiveSamples / (float)windowSize;

	return createBinaryStatus(statusLower, statusHigher, splitValue, windowSize);
}