#include "MRNode.h"
#include "MRNodeImpl.h"

MRSinkNode::MRSinkNode(df::ISinkNode * node, df::IDFSink * sink) : node_(node), sink_(sink)
{
	snkImpl = new MRSinkNodeImpl(this, node->inputsConnected());
}

MRSinkNode::~MRSinkNode()
{
	delete snkImpl;
}

void MRSinkNode::process()
{
	snkImpl->consumeInputs();
	sink_->consume();
}

void MRSinkNode::pause()
{
	std::lock_guard<std::mutex> lock(mutex);
	nodeImpl.pause();
}

void MRSinkNode::resume()
{
	std::lock_guard<std::mutex> lock(mutex);
	nodeImpl.resume();
}

void MRSinkNode::tryPause()
{
	nodeImpl.tryPause();
}

const bool MRSinkNode::paused() const
{
	std::lock_guard<std::mutex> lock(mutex);
	return nodeImpl.paused();
}

void MRSinkNode::updateSnk()
{
	std::lock_guard<std::mutex> lock(mutex);
	snkImpl->updateSnk();
}

void MRSinkNode::reset()
{
	sink_->reset();
	snkImpl->resetInputs();
}

void MRSinkNode::lockSnkProcessing()
{
	snkImpl->wait();
}

void MRSinkNode::unlockSnkProcessing()
{
	snkImpl->wakeUp();
}

void MRSinkNode::addInputPin(MRInputPin * pin)
{
	snkImpl->addPin(pin);
}

MRInputPin * MRSinkNode::inputPin(size_type idx)
{
	return snkImpl->pin(idx);
}

const MRInputPin * MRSinkNode::inputPin(size_type idx) const
{
	return snkImpl->pin(idx);
}

const MRSinkNode::size_type MRSinkNode::inputSize() const
{
	return snkImpl->size();
}

const bool MRSinkNode::inputEmpty() const
{
	return snkImpl->empty();
}

MRSourceNode::MRSourceNode(df::ISourceNode * node, df::IDFSource * source) : node_(node), source_(source)
{
	srcImpl = new MRSourceNodeImpl(this, node->outputsConnected());
}

MRSourceNode::~MRSourceNode()
{
	delete srcImpl;
}

void MRSourceNode::process()
{
	source_->produce();
	srcImpl->updateOutputs();
}

void MRSourceNode::pause()
{
	std::lock_guard<std::mutex> lock(mutex);
	nodeImpl.pause();
}

void MRSourceNode::resume()
{
	std::lock_guard<std::mutex> lock(mutex);
	nodeImpl.resume();
}

void MRSourceNode::tryPause()
{
	nodeImpl.tryPause();
}

const bool MRSourceNode::paused() const
{
	std::lock_guard<std::mutex> lock(mutex);
	return nodeImpl.paused();
}

void MRSourceNode::updateSrc()
{
	std::lock_guard<std::mutex> lock(mutex);
	srcImpl->updateSrc();
}

void MRSourceNode::reset()
{
	std::lock_guard<std::mutex> lock(mutex);
	source_->reset();
	srcImpl->resetOutputs();
}

void MRSourceNode::lockSrcProcessing()
{
	srcImpl->wait();
}

void MRSourceNode::unlockSrcProcessing()
{
	srcImpl->wakeUp();
}

void MRSourceNode::addOutputPin(MROutputPin * pin)
{
	srcImpl->addPin(pin);
}

MROutputPin * MRSourceNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const MROutputPin * MRSourceNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const MRSourceNode::size_type MRSourceNode::outputSize() const
{
	return srcImpl->size();
}

const bool MRSourceNode::outputEmpty() const
{
	return srcImpl->empty();
}

MRProcessingNode::MRProcessingNode(df::IProcessingNode * node, df::IDFProcessor * processor) : node_(node), processor_(processor)
{
	snkImpl = new MRSinkNodeImpl(this, node->inputsConnected());
	srcImpl = new MRSourceNodeImpl(this, node->outputsConnected());
}

MRProcessingNode::~MRProcessingNode()
{
	delete snkImpl;
	delete srcImpl;
}

void MRProcessingNode::process()
{
	snkImpl->consumeInputs();
	processor_->process();
	srcImpl->updateOutputs();
}

void MRProcessingNode::tryPause()
{
	nodeImpl.tryPause();
}

void MRProcessingNode::pause()
{
	std::lock_guard<std::mutex> lock(mutex);
	nodeImpl.pause();
}

void MRProcessingNode::resume()
{
	std::lock_guard<std::mutex> lock(mutex);
	nodeImpl.resume();
}

const bool MRProcessingNode::paused() const
{
	std::lock_guard<std::mutex> lock(mutex);
	return nodeImpl.paused();
}

void MRProcessingNode::updateSnk()
{
	std::lock_guard<std::mutex> lock(mutex);
	snkImpl->updateSnk();
}

void MRProcessingNode::updateSrc()
{
	std::lock_guard<std::mutex> lock(mutex);
	srcImpl->updateSrc();
}

void MRProcessingNode::reset()
{
	std::lock_guard<std::mutex> lock(mutex);
	processor_->reset();
	snkImpl->resetInputs();
	srcImpl->resetOutputs();
}

void MRProcessingNode::lockSnkProcessing()
{
	snkImpl->wait();
}

void MRProcessingNode::unlockSnkProcessing()
{
	snkImpl->wakeUp();
}

void MRProcessingNode::addInputPin(MRInputPin * pin)
{
	snkImpl->addPin(pin);
}

MRInputPin * MRProcessingNode::inputPin(size_type idx)
{	
	return snkImpl->pin(idx);
}

const MRInputPin * MRProcessingNode::inputPin(size_type idx) const
{	
	return snkImpl->pin(idx);
}

const MRProcessingNode::size_type MRProcessingNode::inputSize() const
{
	return snkImpl->size();
}

const bool MRProcessingNode::inputEmpty() const
{
	return snkImpl->empty();
}

void MRProcessingNode::lockSrcProcessing()
{
	srcImpl->wait();
}

void MRProcessingNode::unlockSrcProcessing()
{
	srcImpl->wakeUp();
}

void MRProcessingNode::addOutputPin(MROutputPin * pin)
{
	srcImpl->addPin(pin);
}

MROutputPin * MRProcessingNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const MROutputPin * MRProcessingNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const MRProcessingNode::size_type MRProcessingNode::outputSize() const
{
	return srcImpl->size();
}

const bool MRProcessingNode::outputEmpty() const
{
	return srcImpl->empty();
}