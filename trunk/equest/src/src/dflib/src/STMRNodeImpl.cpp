#include "STMRNodeImpl.h"
#include "ISTMRNode.h"
#include "STMRPin.h"
#include <algorithm>
#include <stdexcept>
#include "utils/Debug.h"

STMRNodeImpl::STMRNodeImpl() : paused_(false)
{
}

void STMRNodeImpl::pause()
{
	if (paused_ == false)
	{
		paused_ = true;
	}
}

void STMRNodeImpl::resume()
{
	if (paused_ == true)
	{
		paused_ = false;
	}
}

const bool STMRNodeImpl::paused() const
{
	return paused_;
}

void STMRNodeImpl::tryPause()
{
		//pauseVariable.wait(lock);
}

STMRSourceNodeImpl::STMRSourceNodeImpl(ISTMRSourceNode * source, unsigned int toConsume) : source_(source), toConsume(toConsume), currentlyConsumed(0)
{
}

STMRSourceNodeImpl::~STMRSourceNodeImpl()
{
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it){
		delete *it;
	}
}

void STMRSourceNodeImpl::updateSrc()
{
	++currentlyConsumed;
	if (currentlyConsumed == toConsume)
	{
		currentlyConsumed = 0;
		//source_->unlockSrcProcessing();
	}
}

void STMRSourceNodeImpl::updateOutputs()
{
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it)
	{
		(*it)->update();
	}
}

void STMRSourceNodeImpl::resetOutputs()
{
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it)
	{
		(*it)->reset();
	}
}

void STMRSourceNodeImpl::addPin(STMROutputPin * pin)
{
	if (std::find(outputPins.begin(), outputPins.end(), pin) != outputPins.end())
	{
		throw std::runtime_error("Pin already added to node");
	}

	outputPins.push_back(pin);
}

const bool STMRSourceNodeImpl::empty() const
{
	return outputPins.empty();
}

const STMRSourceNodeImpl::size_type STMRSourceNodeImpl::size() const
{
	return outputPins.size();
}

STMROutputPin * STMRSourceNodeImpl::pin(size_type idx)
{
	return outputPins[idx];
}

const STMROutputPin * STMRSourceNodeImpl::pin(size_type idx) const
{
	return outputPins[idx];
}

void STMRSourceNodeImpl::wait()
{	

}

void STMRSourceNodeImpl::wakeUp()
{

}

STMRSinkNodeImpl::STMRSinkNodeImpl(ISTMRSinkNode * sink, unsigned int toConsume) : sink_(sink), toConsume(toConsume), readyToConsume(0)
{
}

STMRSinkNodeImpl::~STMRSinkNodeImpl()
{
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it){
		delete *it;
	}
}

void STMRSinkNodeImpl::updateSnk()
{
	++readyToConsume;
	UTILS_ASSERT(readyToConsume <= toConsume);
	/*if (readyToConsume == toConsume)
	{
		readyToConsume = 0;
		sink_->unlockSnkProcessing();
	}*/
}

void STMRSinkNodeImpl::consumeInputs()
{
	UTILS_ASSERT(readyToConsume == toConsume);
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it)
	{
		(*it)->consumeData();
	}
	readyToConsume = 0;
}

void STMRSinkNodeImpl::resetInputs()
{
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it)
	{
		(*it)->reset();
	}
}

void STMRSinkNodeImpl::addPin(STMRInputPin * pin)
{
	if (std::find(inputPins.begin(), inputPins.end(), pin) != inputPins.end())
	{
		throw std::runtime_error("Pin already added to node");
	}

	inputPins.push_back(pin);
}

const bool STMRSinkNodeImpl::empty() const
{
	return inputPins.empty();
}

const STMRSinkNodeImpl::size_type STMRSinkNodeImpl::size() const
{
	return inputPins.size();
}

STMRInputPin * STMRSinkNodeImpl::pin(size_type idx)
{
	return inputPins[idx];
}

const STMRInputPin * STMRSinkNodeImpl::pin(size_type idx) const
{
	return inputPins[idx];
}

void STMRSinkNodeImpl::wait()
{
}

void STMRSinkNodeImpl::wakeUp()
{
}

bool STMRSinkNodeImpl::hasSomethingToProcess() const
{
	return (readyToConsume == toConsume);
}
