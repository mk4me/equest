#include "MRNodeImpl.h"
#include "IMRNode.h"
#include "MRPin.h"
#include <algorithm>
#include <stdexcept>

MRNodeImpl::MRNodeImpl() : paused_(false)
{
}

void MRNodeImpl::pause()
{
	if (paused_ == false)
	{
		paused_ = true;
		syncPolicy.lock();
	}
}

void MRNodeImpl::resume()
{
	if (paused_ == true)
	{
		paused_ = false;
		syncPolicy.unlock();
	}
}

const bool MRNodeImpl::paused() const
{
	return paused_;
}

void MRNodeImpl::tryPause()
{
	std::unique_lock<std::mutex> lock(syncPolicy);
	//pauseVariable.wait(lock);
}

MRSourceNodeImpl::MRSourceNodeImpl(IMRSourceNode * source, unsigned int toConsume) : source_(source), toConsume(toConsume), currentlyConsumed(0)
{
}

MRSourceNodeImpl::~MRSourceNodeImpl()
{
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it){
		delete *it;
	}
}

void MRSourceNodeImpl::updateSrc()
{
	++currentlyConsumed;
	if (currentlyConsumed == toConsume)
	{
		currentlyConsumed = 0;
		source_->unlockSrcProcessing();
	}
}

void MRSourceNodeImpl::updateOutputs()
{
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it)
	{
		(*it)->update();
	}
}

void MRSourceNodeImpl::resetOutputs()
{
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it)
	{
		(*it)->reset();
	}
}

void MRSourceNodeImpl::addPin(MROutputPin * pin)
{
	if (std::find(outputPins.begin(), outputPins.end(), pin) != outputPins.end())
	{
		throw std::runtime_error("Pin already added to node");
	}

	outputPins.push_back(pin);
}

const bool MRSourceNodeImpl::empty() const
{
	return outputPins.empty();
}

const MRSourceNodeImpl::size_type MRSourceNodeImpl::size() const
{
	return outputPins.size();
}

MROutputPin * MRSourceNodeImpl::pin(size_type idx)
{
	return outputPins[idx];
}

const MROutputPin * MRSourceNodeImpl::pin(size_type idx) const
{
	return outputPins[idx];
}

void MRSourceNodeImpl::wait()
{	
	std::unique_lock<std::mutex> lock(syncPolicy);
	wait_.wait(lock);
}

void MRSourceNodeImpl::wakeUp()
{
	wait_.notify_one();
}

MRSinkNodeImpl::MRSinkNodeImpl(IMRSinkNode * sink, unsigned int toConsume) : sink_(sink), toConsume(toConsume), readyToConsume(0)
{
}

MRSinkNodeImpl::~MRSinkNodeImpl()
{
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it){
		delete *it;
	}
}

void MRSinkNodeImpl::updateSnk()
{
	++readyToConsume;
	if (readyToConsume == toConsume)
	{
		readyToConsume = 0;
		sink_->unlockSnkProcessing();
	}
}

void MRSinkNodeImpl::consumeInputs()
{
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it)
	{
		(*it)->consumeData();
	}
}

void MRSinkNodeImpl::resetInputs()
{
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it)
	{
		(*it)->reset();
	}
}

void MRSinkNodeImpl::addPin(MRInputPin * pin)
{
	if (std::find(inputPins.begin(), inputPins.end(), pin) != inputPins.end())
	{
		throw std::runtime_error("Pin already added to node");
	}

	inputPins.push_back(pin);
}

const bool MRSinkNodeImpl::empty() const
{
	return inputPins.empty();
}

const MRSinkNodeImpl::size_type MRSinkNodeImpl::size() const
{
	return inputPins.size();
}

MRInputPin * MRSinkNodeImpl::pin(size_type idx)
{
	return inputPins[idx];
}

const MRInputPin * MRSinkNodeImpl::pin(size_type idx) const
{
	return inputPins[idx];
}

void MRSinkNodeImpl::wait()
{
	std::unique_lock<std::mutex> lock(sync);
	wait_.wait(lock);
}

void MRSinkNodeImpl::wakeUp()
{
	wait_.notify_one();
}
