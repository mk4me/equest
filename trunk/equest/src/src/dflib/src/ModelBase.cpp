#include <dflib/ModelBase.h>

namespace df {


	ModelBase::~ModelBase() {}


	ModelBase::ModelBase() {}

	void ModelBase::attachNode(INode * node)
	{
		node->setModel(this);
	}


	void ModelBase::detachNode(INode * node)
	{
		node->setModel(nullptr);
	}

	void ModelBase::addConnection(IPin * pin, IConnection * connection)
	{
		pin->addConnection(connection);
	}

	void ModelBase::removeConnection(IPin * pin, IConnection * connection)
	{
		pin->removeConnection(connection);
	}

	void ModelBase::connectInput(ISinkNode * node, IInputPin * pin)
	{
		node->connectInput(pin);
	}

	void ModelBase::disconnectInput(ISinkNode * node, IInputPin * pin)
	{
		node->disconnectInput(pin);
	}

	void ModelBase::connectOutput(ISourceNode * node, IOutputPin * pin)
	{
		node->connectOutput(pin);
	}

	void ModelBase::disconnectOutput(ISourceNode * node, IOutputPin * pin)
	{
		node->disconnectOutput(pin);
	}

}