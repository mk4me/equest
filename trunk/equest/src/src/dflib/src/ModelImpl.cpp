#include <dflib/ModelImpl.h>
#include <dflib/IPin.h>
#include <dflib/INode.h>
#include <dflib/IConnection.h>

namespace df
{
	ModelImpl::Nodes::iterator ModelImpl::find(const INode * node)
	{
		auto ret = std::find(nodes_.begin(), nodes_.end(), node);
		if(exists(ret) == false){
			throw NodeInModelEditionException("Node not registered in model");
		}

		return ret;
	}

	ModelImpl::Connections::iterator ModelImpl::find( IConnection * connection ) 
	{
		return std::find(connections_.begin(), connections_.end(), connection);
	}

	ModelImpl::Connections::iterator ModelImpl::verifyConnectionForEdition( IConnection * connection ) 
	{
		auto ret = find(connection);
		if(exists(ret) == false)
		{
			throw PinsConnectionException("Connection not exist");
		}

		return ret;
	}

	void ModelImpl::verifyNodeForAddition(const INode* node)
	{
		if(node->connected() == true)
		{
			throw NodeInModelEditionException("Node has already connections");
		}
	}

	ModelImpl::ModelImpl()
	{

	}

	ModelImpl::~ModelImpl()
	{

	}

	void ModelImpl::verifySourceNodeForAddition(ISourceNode * src)
	{
		if(src->outputSize() == 0)
		{
			throw NodePinsConfigurationException("Source node without output pins");
		}
	}

	void ModelImpl::verifySinkNodeForAddition(ISinkNode * snk)
	{
		if(snk->inputSize() == 0)
		{
			throw NodePinsConfigurationException("Sink node without input pins");
		}
	}

	void ModelImpl::addNode(ISourceNode * src)
	{
		sourceNodes_.push_back(src);
		nodes_.push_back(src);
	}

	void ModelImpl::addNode(ISinkNode * snk)
	{
		sinkNodes_.push_back(snk);
		nodes_.push_back(snk);
	}

	void ModelImpl::addNode(IProcessingNode * proc)
	{
		processingNodes_.push_back(proc);
		nodes_.push_back(proc);
	}

	ModelImpl::SourceNodes::iterator ModelImpl::find(ISourceNode * src)
	{
		return std::find(sourceNodes_.begin(), sourceNodes_.end(), src);
	}

	ModelImpl::SinkNodes::iterator ModelImpl::find(ISinkNode * snk)
	{
		return std::find(sinkNodes_.begin(), sinkNodes_.end(), snk);
	}

	ModelImpl::ProcessingNodes::iterator ModelImpl::find(IProcessingNode * proc)
	{
		return std::find(processingNodes_.begin(), processingNodes_.end(), proc);
	}

	const bool ModelImpl::exists(Nodes::iterator nIT)
	{
		return nIT != nodes_.end();
	}

	const bool ModelImpl::exists(SourceNodes::iterator srcIT)
	{
		return srcIT != sourceNodes_.end();
	}

	const bool ModelImpl::exists(SinkNodes::iterator snkIT)
	{
		return snkIT != sinkNodes_.end();
	}

	const bool ModelImpl::exists(ProcessingNodes::iterator procIT)
	{
		return procIT != processingNodes_.end();
	}

	const bool ModelImpl::exists(Connections::iterator connIT)
	{
		return connIT != connections_.end();
	}

	void ModelImpl::removeNode(SourceNodes::iterator srcIT, Nodes::iterator nIT)
	{
		sourceNodes_.erase(srcIT);
		nodes_.erase(nIT);
	}

	void ModelImpl::removeNode(SinkNodes::iterator snkIT, Nodes::iterator nIT)
	{
		sinkNodes_.erase(snkIT);
		nodes_.erase(nIT);
	}

	void ModelImpl::removeNode(ProcessingNodes::iterator procIT, Nodes::iterator nIT)
	{
		processingNodes_.erase(procIT);
		nodes_.erase(nIT);
	}

	//! Usuwa wszystkie węzły i połączenia z modelu
	void ModelImpl::removeAllNodes()
	{
		SourceNodes().swap(sourceNodes_);
		ProcessingNodes().swap(processingNodes_);
		SinkNodes().swap(sinkNodes_);
		Nodes().swap(nodes_);
		Connections().swap(connections_);
	}

	//! \param src Pin źródłowy (wyjściowy)
	//! \param src Pin docelowy (wejściowy)
	//! \return Czy można połączyć piny ze względu na obowiązujące reguły
	const bool ModelImpl::canConnect(const IOutputPin * src, const IInputPin * dest) const
	{
		if(src->node() == nullptr || dest->node() == nullptr)
		{
			throw PinsConnectionException("Connecting raw pins without nodes");
		}

		if(src->node()->model() == nullptr || dest->node()->model() == nullptr)
		{
			throw PinsConnectionException("Connecting pins of nodes not managed by the model");
		}

		if(src->node()->model() != dest->node()->model())
		{
			throw PinsConnectionException("Connecting pins of nodes from different models");
		}

		if(src->connected() == true && dest->connected() == true)
		{
			const auto s = dest->connectionsSize();
			for(std::size_t i = 0; i < s; ++i)
			{
				if(dest->connection(i)->source() == src)
				{
					throw PinsConnectionException("Pins already connected");
				}
			}
		}

		return dest->pinCompatible(src);
	}

	void ModelImpl::addConnection(IConnection * connection)
	{
		connections_.push_back(connection);
	}

	//! \param connection Połączenie do usunięcia
	void ModelImpl::removeConnection(Connections::iterator connIT)
	{
		connections_.erase(connIT);
	}

	//! Usuwa wszystkie połączenia w modelu
	void ModelImpl::removeAllConnections()
	{
		Connections().swap(connections_);
	}

	const ModelImpl::size_type ModelImpl::nodesSize() const
	{
		return nodes_.size();
	}

	const bool ModelImpl::nodesEmpty() const
	{
		return nodes_.empty();
	}

	const INode * ModelImpl::node(size_type idx) const
	{
		return nodes_[idx];
	}

	INode * ModelImpl::node(size_type idx)
	{		
		return nodes_[idx];
	}

	//! \return Wszystkie węzły modelu
	const ModelImpl::size_type ModelImpl::sourcesSize() const
	{
		return sourceNodes_.size();
	}

	const bool ModelImpl::sourcesEmpty() const
	{
		return sourceNodes_.empty();
	}

	const ISourceNode * ModelImpl::source(size_type idx) const
	{
		return sourceNodes_[idx];
	}

	ISourceNode * ModelImpl::source(size_type idx)
	{
		return sourceNodes_[idx];
	}

	//! \return Wszystkie węzły modelu
	const ModelImpl::size_type ModelImpl::sinksSize() const
	{
		return sinkNodes_.size();
	}

	const bool ModelImpl::sinksEmpty() const
	{
		return sinkNodes_.empty();
	}

	const ISinkNode * ModelImpl::sink(size_type idx) const
	{
		return sinkNodes_[idx];
	}

	ISinkNode * ModelImpl::sink(size_type idx)
	{
		return sinkNodes_[idx];
	}

	//! \return Wszystkie węzły modelu
	const ModelImpl::size_type ModelImpl::processorsSize() const
	{
		return processingNodes_.size();
	}

	const bool ModelImpl::processorsEmpty() const
	{
		return processingNodes_.empty();
	}

	const IProcessingNode * ModelImpl::processor(size_type idx) const
	{
		return processingNodes_[idx];
	}

	IProcessingNode * ModelImpl::processor(size_type idx)
	{
		return processingNodes_[idx];
	}

	//! \return Wszystkie połączenia w modelu
	const ModelImpl::size_type ModelImpl::connectionsSize() const
	{
		return connections_.size();
	}

	const bool ModelImpl::connectionsEmpty() const
	{
		return connections_.empty();
	}

	const IConnection * ModelImpl::connection(size_type idx) const
	{
		return connections_[idx];
	}

	IConnection * ModelImpl::connection(size_type idx)
	{
		return connections_[idx];
	}
}
