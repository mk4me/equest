/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   11:25
    filename: ModelImpl.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MODELIMPL_H__
#define HEADER_GUARD___MODELIMPL_H__

#include <vector>
#include <algorithm>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

	class INode;
	class ISourceNode;
	class ISinkNode;
	class IProcessingNode;
	class IInputPin;
	class IOutputPin;
	class IConnection;

	// Wyjątek rzucany kiedy naruszymy reguły łączenia
	class BasicConnectionRulesException : public std::runtime_error
	{
	public:
		BasicConnectionRulesException(const char * e) : std::runtime_error(e) {}
	};

	// Wyjątek rzucany kiedy naruszymy reguły łączenia
	class NodePinsConfigurationException : public std::runtime_error
	{
	public:
		NodePinsConfigurationException(const char * e) : std::runtime_error(e) {}
	};

	// Wyjątek rzucany kiedy naruszymy reguły łączenia
	class PinsConnectionException : public std::runtime_error
	{
	public:
		PinsConnectionException(const char * e) : std::runtime_error(e) {}
	};

	// Wyjątek rzucany kiedy naruszymy reguły łączenia
	class NodeInModelEditionException : public std::runtime_error
	{
	public:
		NodeInModelEditionException(const char * e) : std::runtime_error(e) {}
	};


//! Wzorzec klasy będącej podstawą implementacji modelu. Pozwala się parametryzowac dla typów węzła
//! bazowego, źródłowego, sinka, procesora, pinów wejściowych, wyjściowych, połączenia.
class ModelImpl
{
private:

	typedef std::vector<INode*> Nodes;
	//! Typ reprezentujący zbiór węzłów
	typedef std::vector<ISinkNode*> SinkNodes;
	//! Typ reprezentujący zbiór węzłów
	typedef std::vector<ISourceNode*> SourceNodes;
	//! Typ reprezentujący zbiór węzłów
	typedef std::vector<IProcessingNode*> ProcessingNodes;
	//! Typ opisujący zbiór połączeń
	typedef std::vector<IConnection*> Connections;

public:

	using size_type = Nodes::size_type;

public:

	ModelImpl();

	~ModelImpl();

	static void verifyNodeForAddition(const INode* node);

	static void verifySourceNodeForAddition(ISourceNode * src);

	static void verifySinkNodeForAddition(ISinkNode * snk);

	void addNode(ISourceNode * src);

	void addNode(ISinkNode * snk);

	void addNode(IProcessingNode * proc);

	SourceNodes::iterator find(ISourceNode * src);

	SinkNodes::iterator find(ISinkNode * snk);

	ProcessingNodes::iterator find(IProcessingNode * proc);

	Nodes::iterator find(const INode * node);

	Connections::iterator find( IConnection * connection );

	const bool exists(Nodes::iterator nIT);
	const bool exists(SourceNodes::iterator srcIT);
	const bool exists(SinkNodes::iterator snkIT);
	const bool exists(ProcessingNodes::iterator procIT);
	const bool exists(Connections::iterator connIT);

	Connections::iterator verifyConnectionForEdition( IConnection * connection );

	void removeNode(SourceNodes::iterator srcIT, Nodes::iterator nIT);

	void removeNode(SinkNodes::iterator snkIT, Nodes::iterator nIT);

	void removeNode(ProcessingNodes::iterator procIT, Nodes::iterator nIT);

	//! Usuwa wszystkie węzły i połączenia z modelu
	void removeAllNodes();

	//! \param src Pin źródłowy (wyjściowy)
	//! \param src Pin docelowy (wejściowy)
	//! \return Czy można połaczyć piny ze względu na obowiązujące reguły
	const bool canConnect(const IOutputPin * src, const IInputPin * dest) const;

	void addConnection(IConnection * connection);

	//! \param connection Połączenie do usunięcia
	void removeConnection(Connections::iterator connIT);

	//! Usuwa wszystkie połączenia w modelu
	void removeAllConnections();

	const size_type nodesSize() const;

	const bool nodesEmpty() const;

	const INode * node(size_type idx) const;

	INode * node(size_type idx);

	//! \return Wszystkie węzły modelu
	const size_type sourcesSize() const;

	const bool sourcesEmpty() const;

	const ISourceNode * source(size_type idx) const;

	ISourceNode * source(size_type idx);

	//! \return Wszystkie węzły modelu
	const size_type sinksSize() const;

	const bool sinksEmpty() const;

	const ISinkNode * sink(size_type idx) const;

	ISinkNode * sink(size_type idx);

	//! \return Wszystkie węzły modelu
	const size_type processorsSize() const;

	const bool processorsEmpty() const;

	const IProcessingNode * processor(size_type idx) const;

	IProcessingNode * processor(size_type idx);

	//! \return Wszystkie połączenia w modelu
	const size_type connectionsSize() const;

	const bool connectionsEmpty() const;

	const IConnection * connection(size_type idx) const;

	IConnection * connection(size_type idx);

private:

	//! Zbiór wszystkich węzłów modelu
	Nodes nodes_;
	//! Zbiór źródeł modelu
	SourceNodes sourceNodes_;
	//! Zbiór sinków modeu
	SinkNodes sinkNodes_;
	//! Zbiór elementów przetwarzających modelu
	ProcessingNodes processingNodes_;
	//! Zbiór wszystkich połączeń modelu
	Connections connections_;
};

}

#endif	//	HEADER_GUARD___MODELIMPL_H__
