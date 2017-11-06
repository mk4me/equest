/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   11:24
    filename: Model.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MODEL_H__
#define HEADER_GUARD___MODEL_H__

#include <stdexcept>
#include <dflib/ModelBase.h>
#include <dflib/IModel.h>
#include <dflib/ModelImpl.h>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

//! Implementacja modelu
class Model : public IModel, public ModelBase
{
private:

	void verifyNodeForEdition(const INode * node) const;

	void verifyNodeForAddition(const INode * node) const;

	void innerRemoveConnection(IConnection * connection);
	void innerDisconnectNode(INode * node);

	void innerRemoveNode(ISourceNode * node);
	void innerRemoveNode(ISinkNode * node);
	void innerRemoveNode(IProcessingNode * node);

	void innerDisconnectNode(ISinkNode * node);
	void innerDisconnectNode(ISourceNode * node);
	void innerDisconnectNode(IProcessingNode * node);

	static void disconnectConnection(IConnection * connection);

public:

	Model();

	~Model();

	virtual void addNode(ISourceNode * src);

	virtual void addNode(ISinkNode * snk);

	virtual void addNode(IProcessingNode * proc);

	virtual void removeNode(ISourceNode * node);
	virtual void removeNode(ISinkNode * node);
	virtual void removeNode(IProcessingNode * node);

	//! Usuwa wszystkie węzły i połączenia z modelu
	virtual void removeAllNodes();

	//! \param src Pin źródłowy (wyjściowy)
	//! \param src Pin docelowy (wejściowy)
	//! \return Czy można połaczyć piny ze względu na obowiązujące reguły
	virtual const bool canConnect(const IOutputPin * src, const IInputPin * dest) const;

	//! \param connection Połączenie dodawane do modelu
	virtual void addConnection(IConnection * connection);

	//! \param connection Połączenie do usunięcia
	virtual void removeConnection(IConnection * connection);

	//! \param Wezeł do rozłączenia - wszystke jego połaczenia zostają usuniete
	virtual void disconnectNode(ISourceNode * node);	
	//! \param Wezeł do rozłączenia - wszystke jego połaczenia zostają usuniete
	virtual void disconnectNode(ISinkNode * node);	
	//! \param Wezeł do rozłączenia - wszystke jego połaczenia zostają usuniete
	virtual void disconnectNode(IProcessingNode * node);

	//! Usuwa wszystkie połączenia w modelu
	virtual void removeAllConnections();

	virtual const size_type nodesSize() const;

	virtual const bool nodesEmpty() const;

	virtual const INode * node(size_type idx) const;

	virtual INode * node(size_type idx);

	//! \return Wszystkie węzły modelu
	virtual const size_type sourcesSize() const;

	virtual const bool sourcesEmpty() const;

	virtual const ISourceNode * source(size_type idx) const;

	virtual ISourceNode * source(size_type idx);

	//! \return Wszystkie węzły modelu
	virtual const size_type sinksSize() const;

	virtual const bool sinksEmpty() const;

	virtual const ISinkNode * sink(size_type idx) const;

	virtual ISinkNode * sink(size_type idx);

	//! \return Wszystkie węzły modelu
	virtual const size_type processorsSize() const;

	virtual const bool processorsEmpty() const;

	virtual const IProcessingNode * processor(size_type idx) const;

	virtual IProcessingNode * processor(size_type idx);

	//! \return Wszystkie połączenia w modelu
	virtual const size_type connectionsSize() const;

	virtual const bool connectionsEmpty() const;

	virtual const IConnection * connection(size_type idx) const;

	virtual IConnection * connection(size_type idx);

private:

	ModelImpl modelImpl;
};

}

#endif	//	HEADER_GUARD___MODEL_H__
