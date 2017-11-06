/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:59
    filename: IModel.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IMODEL_H__
#define HEADER_GUARD___IMODEL_H__

#include <dflib/ModelBase.h>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

//! Forward declarations
class ISourceNode;
class ISinkNode;
class IProcessingNode;
class IInputPin;
class IOutputPin;
class IConnection;

//! Interfejs obiektu czytającego stan modelu
class IModelReader
{
public:

	typedef unsigned int size_type;

public:

	//! \return Ilośc węzłów źródłowych
	virtual const size_type sourcesSize() const = 0;
	//! \return Czy są jakies źródła
	virtual const bool sourcesEmpty() const = 0;
	//! \param idx Indeks węzła źródłowego, który chcemy pobrać
	//! \return Węzeł źrółowy o zadanym indeksie
	virtual const ISourceNode * source(size_type idx) const = 0;
	//! \param idx Indeks węzła źródłowego, który chcemy pobrać
	//! \return Węzeł źrółowy o zadanym indeksie
	virtual ISourceNode * source(size_type idx) = 0;

	//! \return Ilośc węzłów sink
	virtual const size_type sinksSize() const = 0;
	//! \return Czy są jakies sinki
	virtual const bool sinksEmpty() const = 0;
	//! \param idx Indeks węzła sink, który chcemy pobrać
	//! \return Węzeł sink o zadanym indeksie
	virtual const ISinkNode * sink(size_type idx) const = 0;
	//! \param idx Indeks węzła sink, który chcemy pobrać
	//! \return Węzeł sink o zadanym indeksie
	virtual ISinkNode * sink(size_type idx) = 0;

	//! \return Ilośc węzłów przetwarzających
	virtual const size_type processorsSize() const = 0;
	//! \return Czy są jakies węzły przetwarzajace
	virtual const bool processorsEmpty() const = 0;
	//! \param idx Indeks węzła przetwarzającego, który chcemy pobrać
	//! \return Węzeł przetwarzający o zadanym indeksie
	virtual const IProcessingNode * processor(size_type idx) const = 0;
	//! \param idx Indeks węzła przetwarzającego, który chcemy pobrać
	//! \return Węzeł przetwarzający o zadanym indeksie
	virtual IProcessingNode * processor(size_type idx) = 0;

	//! \return Ilośc połączeń w modelu
	virtual const size_type connectionsSize() const = 0;
	//! \return Czy w modelu sa jakieś połączenia
	virtual const bool connectionsEmpty() const = 0;
	//! \param idx Indeks połaczenia które chcemy pobrać
	//! \return Połączenie o zadanym indeksie
	virtual const IConnection * connection(size_type idx) const = 0;
	//! \param idx Indeks połaczenia które chcemy pobrać
	//! \return Połączenie o zadanym indeksie
	virtual IConnection * connection(size_type idx) = 0;

	//! \return Ilość wszystkich węzłów w modelu
	virtual const size_type nodesSize() const = 0;
	//! \return Czy sa jakieś węzły w modelu
	virtual const bool nodesEmpty() const = 0;
	//! \param idx Indeks węzła który chcemy pobrać
	//! \return Węzeł o zadanym indeksie
	virtual const INode * node(size_type idx) const = 0;
	//! \param idx Indeks węzła który chcemy pobrać
	//! \return Węzeł o zadanym indeksie
	virtual INode * node(size_type idx) = 0;
};


//! Interfejs edytora modelu. Pozwala dodawać/usuwać węzły,
//! edytować połączenia między węzłami
class IModelEditor
{
public:
	//! \param node Węzeł źródłowy który chcemy dodać
	virtual void addNode(ISourceNode * node) = 0;
	//! \param node Węzeł sink który chcemy dodać
	virtual void addNode(ISinkNode * node) = 0;
	//! \param node Węzeł przetwarzający który chcemy dodać
	virtual void addNode(IProcessingNode * node) = 0;

	//! \param node Węzeł który chcemy usunąć
	virtual void removeNode(ISourceNode * node) = 0;
	//! \param node Węzeł który chcemy usunąć
	virtual void removeNode(ISinkNode * node) = 0;
	//! \param node Węzeł który chcemy usunąć
	virtual void removeNode(IProcessingNode * node) = 0;

	//! Usuwa wszystkie węzły i połączenia z modelu
	virtual void removeAllNodes() = 0;

	//! \param connection Połączenie dodawane do modelu
	virtual void addConnection(IConnection * connection) = 0;

	//! \param src Pin źródłowy (wyjściowy)
	//! \param src Pin docelowy (wejściowy)
	//! \return Czy można połaczyć piny ze względu na obowiązujące reguły
	virtual const bool canConnect(const IOutputPin * src, const IInputPin * dest) const = 0;

	//! \param connection Połączenie do usunięcia
	virtual void removeConnection(IConnection * connection) = 0;

	//! \param Wezeł do rozłączenia - wszystke jego połaczenia zostają usuniete
	virtual void disconnectNode(ISourceNode * node) = 0;	
	//! \param Wezeł do rozłączenia - wszystke jego połaczenia zostają usuniete
	virtual void disconnectNode(ISinkNode * node) = 0;	
	//! \param Wezeł do rozłączenia - wszystke jego połaczenia zostają usuniete
	virtual void disconnectNode(IProcessingNode * node) = 0;	

	//! Usuwa wszystkie połączenia w modelu
	virtual void removeAllConnections() = 0;	
};

class IModel : public IModelReader, public IModelEditor
{

};

}

#endif	//	HEADER_GUARD___IMODEL_H__
