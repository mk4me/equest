#include "STMRNode.h"
#include "STMRNodeImpl.h"

STMRSinkNode::STMRSinkNode(df::ISinkNode * node, df::IDFSink * sink) : node_(node), sink_(sink)
{
	snkImpl = new STMRSinkNodeImpl(this, node->inputsConnected());
}

STMRSinkNode::~STMRSinkNode()
{
	delete snkImpl;
}

void STMRSinkNode::process()
{
	snkImpl->consumeInputs();
	sink_->consume();
}


void STMRSinkNode::reset()
{
	sink_->reset();
	snkImpl->resetInputs();
}


//void STMRSinkNode::unlockSnkProcessing()
//{
//	process();
//	snkImpl->wakeUp();
//}

void STMRSinkNode::addInputPin(STMRInputPin * pin)
{
	snkImpl->addPin(pin);
}

STMRInputPin * STMRSinkNode::inputPin(size_type idx)
{
	return snkImpl->pin(idx);
}

const STMRInputPin * STMRSinkNode::inputPin(size_type idx) const
{
	return snkImpl->pin(idx);
}

const STMRSinkNode::size_type STMRSinkNode::inputSize() const
{
	return snkImpl->size();
}

const bool STMRSinkNode::inputEmpty() const
{
	return snkImpl->empty();
}

bool STMRSinkNode::hasSomethingToProcess()
{
	return snkImpl->hasSomethingToProcess();
}

void STMRSinkNode::updateSnk()
{
	snkImpl->updateSnk();
}

STMRSourceNode::STMRSourceNode(df::ISourceNode * node, df::IDFSource * source) : node_(node), source_(source)
{
	srcImpl = new STMRSourceNodeImpl(this, node->outputsConnected());
}

STMRSourceNode::~STMRSourceNode()
{
	delete srcImpl;
}

void STMRSourceNode::process()
{
	source_->produce();
	srcImpl->updateOutputs();
}



void STMRSourceNode::reset()
{
	source_->reset();
	srcImpl->resetOutputs();
}



void STMRSourceNode::addOutputPin(STMROutputPin * pin)
{
	srcImpl->addPin(pin);
}

STMROutputPin * STMRSourceNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const STMROutputPin * STMRSourceNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const STMRSourceNode::size_type STMRSourceNode::outputSize() const
{
	return srcImpl->size();
}

const bool STMRSourceNode::outputEmpty() const
{
	return srcImpl->empty();
}

bool STMRSourceNode::hasSomethingToProcess()
{
	return !source_->empty();
}

STMRProcessingNode::STMRProcessingNode(df::IProcessingNode * node, df::IDFProcessor * processor) : node_(node), processor_(processor)
{
	snkImpl = new STMRSinkNodeImpl(this, node->inputsConnected());
	srcImpl = new STMRSourceNodeImpl(this, node->outputsConnected());
}

STMRProcessingNode::~STMRProcessingNode()
{
	delete snkImpl;
	delete srcImpl;
}

void STMRProcessingNode::process()
{
	snkImpl->consumeInputs();
	processor_->process();
	srcImpl->updateOutputs();
}



void STMRProcessingNode::reset()
{
	processor_->reset();
	snkImpl->resetInputs();
	srcImpl->resetOutputs();
}


//void STMRProcessingNode::unlockSnkProcessing()
//{
//	snkImpl->wakeUp();
//	process();
//}

void STMRProcessingNode::addInputPin(STMRInputPin * pin)
{
	snkImpl->addPin(pin);
}

STMRInputPin * STMRProcessingNode::inputPin(size_type idx)
{	
	return snkImpl->pin(idx);
}

const STMRInputPin * STMRProcessingNode::inputPin(size_type idx) const
{	
	return snkImpl->pin(idx);
}

const STMRProcessingNode::size_type STMRProcessingNode::inputSize() const
{
	return snkImpl->size();
}

const bool STMRProcessingNode::inputEmpty() const
{
	return snkImpl->empty();
}


void STMRProcessingNode::addOutputPin(STMROutputPin * pin)
{
	srcImpl->addPin(pin);
}

STMROutputPin * STMRProcessingNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const STMROutputPin * STMRProcessingNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const STMRProcessingNode::size_type STMRProcessingNode::outputSize() const
{
	return srcImpl->size();
}

const bool STMRProcessingNode::outputEmpty() const
{
	return srcImpl->empty();
}

bool STMRProcessingNode::hasSomethingToProcess()
{
	return snkImpl->hasSomethingToProcess();
}

void STMRProcessingNode::updateSnk()
{
	snkImpl->updateSnk();
}
