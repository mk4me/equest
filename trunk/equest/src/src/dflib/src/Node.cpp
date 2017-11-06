#include <dflib/Node.h>

namespace df {

	NodeT_::NodeT_() : model_(nullptr)
	{

	}

	ModelBase * NodeT_::model()
	{
		return model_;
	}

	const ModelBase * NodeT_::model() const
	{
		return model_;
	}

	void NodeT_::setModel(ModelBase * model)
	{
		if(model_ != nullptr){
			if(model != nullptr){
				if(model == model_){
					throw std::runtime_error("Node already attached to desired model");
				}else{
					throw std::runtime_error("Node already attached to other model");
				}
			}else{
				model_ = nullptr;
			}
		}else{
			model_ = model;
		}
	}

	void NodeT_::checkModelForEdition()
	{
		if(model_ != nullptr)
		{
			throw std::runtime_error("Can not edit node attached to model");
		}
	}

	const ModelBase * SourceNode::model() const
	{
		return node_.model();
	}

	ModelBase * SourceNode::model()
	{
		return node_.model();
	}

	const SourceNode::size_type SourceNode::outputsConnected() const
	{
		return nodeImpl.connectedPins();
	}

	const SourceNode::size_type SourceNode::outputSize() const
	{
		return nodeImpl.size();
	}

	const bool SourceNode::outputEmpty() const
	{
		return nodeImpl.empty();
	}

	const IOutputPin * SourceNode::outputPin(size_type idx) const
	{
		return nodeImpl.pin(idx);
	}

	IOutputPin * SourceNode::outputPin(size_type idx)
	{
		return nodeImpl.pin(idx);
	}

	void SourceNode::connectOutput(const IOutputPin * pin)
	{
		nodeImpl.connect(pin);
	}

	void SourceNode::disconnectOutput(const IOutputPin * pin)
	{
		nodeImpl.disconnect(pin);
	}

	void SourceNode::addOutputPin(IOutputPin * pin)
	{
		node_.checkModelForEdition();
		nodeImpl.addPin(pin);
	}

	void SourceNode::removeOutputPin(IOutputPin * pin)
	{
		node_.checkModelForEdition();
		nodeImpl.removePin(pin);
	}

	void SourceNode::setModel(ModelBase * model)
	{
		node_.setModel(model);
	}

	const ModelBase * SinkNode::model() const
	{
		return node_.model();
	}

	ModelBase * SinkNode::model()
	{
		return node_.model();
	}

	const SinkNode::size_type SinkNode::inputsConnected() const
	{
		return nodeImpl.connectedPins();
	}

	const SinkNode::size_type SinkNode::inputSize() const
	{
		return nodeImpl.size();
	}

	const bool SinkNode::inputEmpty() const
	{
		return nodeImpl.empty();
	}

	const IInputPin * SinkNode::inputPin(size_type idx) const
	{
		return nodeImpl.pin(idx);
	}

	IInputPin * SinkNode::inputPin(size_type idx)
	{
		return nodeImpl.pin(idx);
	}

	void SinkNode::connectInput(const IInputPin * pin)
	{
		nodeImpl.connect(pin);
	}

	void SinkNode::disconnectInput(const IInputPin * pin)
	{
		nodeImpl.disconnect(pin);
	}

	void SinkNode::addInputPin(IInputPin * pin)
	{
		node_.checkModelForEdition();
		nodeImpl.addPin(pin);
	}

	void SinkNode::removeInputPin(IInputPin * pin)
	{
		node_.checkModelForEdition();
		nodeImpl.removePin(pin);
	}

	void SinkNode::setModel(ModelBase * model)
	{
		node_.setModel(model);
	}

	const ModelBase * ProcessingNode::model() const
	{
		return node_.model();
	}

	ModelBase * ProcessingNode::model()
	{
		return node_.model();
	}

	const ProcessingNode::size_type ProcessingNode::outputsConnected() const
	{
		return nodeOImpl.connectedPins();
	}

	const ProcessingNode::size_type ProcessingNode::inputsConnected() const
	{
		return nodeIImpl.connectedPins();
	}

	const ProcessingNode::size_type ProcessingNode::inputSize() const
	{
		return nodeIImpl.size();
	}

	const bool ProcessingNode::inputEmpty() const
	{
		return nodeIImpl.empty();
	}

	const ProcessingNode::size_type ProcessingNode::outputSize() const
	{
		return nodeOImpl.size();
	}

	const bool ProcessingNode::outputEmpty() const
	{
		return nodeOImpl.empty();
	}

	const IInputPin * ProcessingNode::inputPin(size_type idx) const
	{
		return nodeIImpl.pin(idx);
	}

	IInputPin * ProcessingNode::inputPin(size_type idx)
	{
		return nodeIImpl.pin(idx);
	}

	const IOutputPin * ProcessingNode::outputPin(size_type idx) const
	{
		return nodeOImpl.pin(idx);
	}

	IOutputPin * ProcessingNode::outputPin(size_type idx)
	{
		return nodeOImpl.pin(idx);
	}

	void ProcessingNode::connectInput(const IInputPin * pin)
	{
		nodeIImpl.connect(pin);
	}

	void ProcessingNode::disconnectInput(const IInputPin * pin)
	{
		nodeIImpl.disconnect(pin);
	}

	void ProcessingNode::connectOutput(const IOutputPin * pin)
	{
		nodeOImpl.connect(pin);
	}

	void ProcessingNode::disconnectOutput(const IOutputPin * pin)
	{
		nodeOImpl.disconnect(pin);
	}

	void ProcessingNode::addOutputPin(IOutputPin * pin)
	{
		node_.checkModelForEdition();
		nodeOImpl.addPin(pin);
	}

	void ProcessingNode::removeOutputPin(IOutputPin * pin)
	{
		node_.checkModelForEdition();
		nodeOImpl.removePin(pin);
	}

	void ProcessingNode::addInputPin(IInputPin * pin)
	{
		node_.checkModelForEdition();
		nodeIImpl.addPin(pin);
	}

	void ProcessingNode::removeInputPin(IInputPin * pin)
	{
		node_.checkModelForEdition();
		nodeIImpl.removePin(pin);
	}

	void ProcessingNode::setModel(ModelBase * model)
	{
		node_.setModel(model);
	}

}
