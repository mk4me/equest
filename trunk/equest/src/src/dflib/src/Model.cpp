#include <dflib/Model.h>
#include <dflib/IConnection.h>
#include <dflib/ModelBase.h>
#include <dflib/ModelImpl.h>
#include <dflib/INode.h>
#include <dflib/IPin.h>


namespace df{

void Model::verifyNodeForEdition(const INode * node) const
{
	if(node->model() == nullptr){
		throw NodeInModelEditionException("Node not in model");
	}else if(node->model() != this){
		throw NodeInModelEditionException("Node belongs to other model");
	}
}

void Model::verifyNodeForAddition(const INode * node) const
{
	ModelImpl::verifyNodeForAddition(node);

	if(node->model() != nullptr){
		if(node->model() == this){
			throw NodeInModelEditionException("Node already in model");
		}else{
			throw NodeInModelEditionException("Node belongs to other model");
		}
	}
}

void Model::disconnectConnection(IConnection * connection)
{
	auto srcPin = connection->source();
	auto srcNode = srcPin->node();

	auto destPin = connection->destination();
	auto destNode = destPin->node();

	ModelBase::removeConnection(srcPin, connection);
	ModelBase::removeConnection(destPin, connection);

	ModelBase::disconnectInput(destNode, destPin);
	ModelBase::disconnectOutput(srcNode, srcPin);
}

void Model::innerRemoveConnection(IConnection * connection)
{
	disconnectConnection(connection);
	modelImpl.removeConnection(modelImpl.find(connection));
}

void Model::innerRemoveNode(ISourceNode * node)
{
	innerDisconnectNode(node);
	detachNode(node);
	modelImpl.removeNode(modelImpl.find(node), modelImpl.find((INode*)node));
}

void Model::innerRemoveNode(ISinkNode * node)
{
	innerDisconnectNode(node);
	detachNode(node);
	modelImpl.removeNode(modelImpl.find(node), modelImpl.find((INode*)node));
}

void Model::innerRemoveNode(IProcessingNode * node)
{
	innerDisconnectNode(node);
	detachNode(node);
	modelImpl.removeNode(modelImpl.find(node), modelImpl.find((INode*)node));
}

void Model::innerDisconnectNode(ISinkNode * node)
{
	for(INode::size_type i = 0; i < node->inputSize(); ++i)
	{
		auto p = node->inputPin(i);
		for(IPin::size_type j = 0; p->connectionsSize(); ++j)
		{
			innerRemoveConnection(p->connection(j));
		}
	}
}

void Model::innerDisconnectNode(ISourceNode * node)
{
	for(INode::size_type i = 0; i < node->outputSize(); ++i)
	{
		auto p = node->outputPin(i);
		for(IPin::size_type j = 0; p->connectionsSize(); ++j)
		{
			innerRemoveConnection(p->connection(j));
		}
	}
}

void Model::innerDisconnectNode(IProcessingNode * node)
{
	innerDisconnectNode((ISinkNode*)node);
	innerDisconnectNode((ISourceNode*)node);
}

Model::Model()
{

}

Model::~Model()
{

}

void Model::addNode(ISourceNode * src)
{
	ModelImpl::verifySourceNodeForAddition(src);
	verifyNodeForAddition(src);

	attachNode(src);
	modelImpl.addNode(src);
}

void Model::addNode(ISinkNode * snk)
{
	ModelImpl::verifySinkNodeForAddition(snk);
	verifyNodeForAddition(snk);

	attachNode(snk);
	modelImpl.addNode(snk);
}

void Model::addNode(IProcessingNode * proc)
{
	ModelImpl::verifySourceNodeForAddition(proc);
	ModelImpl::verifySinkNodeForAddition(proc);
	verifyNodeForAddition(proc);

	attachNode(proc);
	modelImpl.addNode(proc);
}

void Model::removeNode(ISourceNode * node)
{
	verifyNodeForEdition(node);
	
	innerRemoveNode(node);
}

void Model::removeNode(ISinkNode * node)
{
	verifyNodeForEdition(node);

	innerRemoveNode(node);
}

void Model::removeNode(IProcessingNode * node)
{
	verifyNodeForEdition(node);

	innerRemoveNode(node);
}

void Model::removeAllNodes()
{
	for(ModelImpl::size_type i = 0; i < modelImpl.sourcesSize(); ++i)
	{
		innerDisconnectNode(modelImpl.source(i));
	}

	for(ModelImpl::size_type i = 0; i < modelImpl.sinksSize(); ++i)
	{
		innerDisconnectNode(modelImpl.sink(i));
	}

	for(ModelImpl::size_type i = 0; i < modelImpl.processorsSize(); ++i)
	{
		innerDisconnectNode(modelImpl.processor(i));
	}

	modelImpl.removeAllNodes();
}

const bool Model::canConnect(const IOutputPin * src, const IInputPin * dest) const
{
	if(src->node()->model() != this)
	{
		throw PinsConnectionException("Connecting pins of nodes from other model");
	}

	return modelImpl.canConnect(src, dest);
}

void Model::addConnection(IConnection * connection)
{
	if(modelImpl.exists(modelImpl.find(connection)) == true)
	{
		throw PinsConnectionException("Connection already added to model");
	}

	auto src = connection->source();
	auto dest = connection->destination();

	if(src == nullptr || dest == nullptr)
	{
		throw PinsConnectionException("Invalid connection configuration - source or destination not initialized");
	}

	if(canConnect(src, dest) == false)
	{
		throw PinsConnectionException("Connection is violating connection rules");
	}

	ModelBase::connectInput(dest->node(), dest);
	ModelBase::connectOutput(src->node(), src);

	ModelBase::addConnection(src, connection);
	ModelBase::addConnection(dest, connection);

	modelImpl.addConnection(connection);
}

void Model::removeConnection(IConnection * connection)
{
	modelImpl.verifyConnectionForEdition(connection);
	innerRemoveConnection(connection);
}

void Model::disconnectNode(ISourceNode * node)
{
	verifyNodeForEdition(node);
	innerDisconnectNode(node);
}

void Model::disconnectNode(ISinkNode * node)
{
	verifyNodeForEdition(node);
	innerDisconnectNode(node);
}

void Model::disconnectNode(IProcessingNode * node)
{
	verifyNodeForEdition(node);
	innerDisconnectNode(node);
}

void Model::removeAllConnections()
{
	for(ModelImpl::size_type i = 0; i < modelImpl.connectionsSize(); ++i)
	{
		disconnectConnection(modelImpl.connection(i));
	}

	modelImpl.removeAllConnections();
}

const Model::size_type Model::nodesSize() const
{
	return modelImpl.nodesSize();
}

const bool Model::nodesEmpty() const
{
	return modelImpl.nodesEmpty();
}

const INode * Model::node(size_type idx) const
{
	return modelImpl.node(idx);
}

INode * Model::node(size_type idx)
{		
	return modelImpl.node(idx);
}

const Model::size_type Model::sourcesSize() const
{
	return modelImpl.sourcesSize();
}

const bool Model::sourcesEmpty() const
{
	return modelImpl.sourcesEmpty();
}

const ISourceNode * Model::source(size_type idx) const
{
	return modelImpl.source(idx);
}

ISourceNode * Model::source(size_type idx)
{
	return modelImpl.source(idx);
}

const Model::size_type Model::sinksSize() const
{
	return modelImpl.sinksSize();
}

const bool Model::sinksEmpty() const
{
	return modelImpl.sinksEmpty();
}

const ISinkNode * Model::sink(size_type idx) const
{
	return modelImpl.sink(idx);
}

ISinkNode * Model::sink(size_type idx)
{
	return modelImpl.sink(idx);
}

const Model::size_type Model::processorsSize() const
{
	return modelImpl.processorsSize();
}

const bool Model::processorsEmpty() const
{
	return modelImpl.processorsEmpty();
}

const IProcessingNode * Model::processor(size_type idx) const
{
	return modelImpl.processor(idx);
}

IProcessingNode * Model::processor(size_type idx)
{
	return modelImpl.processor(idx);
}

const Model::size_type Model::connectionsSize() const
{
	return modelImpl.connectionsSize();
}

const bool Model::connectionsEmpty() const
{
	return modelImpl.connectionsEmpty();
}

const IConnection * Model::connection(size_type idx) const
{
	return modelImpl.connection(idx);
}

IConnection * Model::connection(size_type idx)
{
	return modelImpl.connection(idx);
}

}