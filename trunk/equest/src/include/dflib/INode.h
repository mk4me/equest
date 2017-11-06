/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   10:52
    filename: INode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___INODE_H__
#define HEADER_GUARD___INODE_H__

#include <vector>
////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

//! Forward declarations

class IConnection;
class ModelBase;

class IInputPin;
class IOutputPin;

class ISourceNode;
class ISinkNode;
class IProcessingNode;

//! Bazowy interfejs wezeła modelu. Opisuje typ węzła i podstawowe informacje
//! na temat jego połączeń.
class INode
{
friend class ModelBase;

public:

	//! Typ węzła
	enum NodeType {
		SOURCE_NODE,	//! źródło
		SINK_NODE,		//! liść
		PROCESSING_NODE	//! procesor
	};

	//! Typ opisujacy ilość pinów
	typedef unsigned int size_type;

public:
    virtual ~INode() {}

public:
	//! \return Typ węzła
	virtual const NodeType type() const = 0;
	//! \return Ilośc podłączonych pinów
	virtual const size_type connectedPins() const = 0;	
	//! \return Czy węzeł podłączony
	const bool connected() const
	{
		return connectedPins() > 0;
	}

	//! \return Model do którego przypisany jest aktualnie węzeł
	virtual ModelBase * model() = 0;
	//! \return Model do którego przypisany jest aktualnie węzeł
	virtual const ModelBase * model() const = 0;

private:
	//! \param model Model do którego aktualnie będzie należał węzeł
	virtual void setModel(ModelBase * model) = 0;	
};

//! Interfejs węzła źrółowego. Posiada on tylko piny wyjściowe.
class ISourceNode : public virtual INode
{
friend class ModelBase;

public:

	//! \return Typ węzła
	virtual const NodeType type() const { return INode::SOURCE_NODE; }

	//! \param idx Indeks pinu wyjsciowego
	//! \return Pin wyjściowy spod wskazanego indeksu
	virtual const IOutputPin * outputPin(size_type idx) const = 0;
	//! \param idx Indeks pinu wyjsciowego
	//! \return Pin wyjściowy spod wskazanego indeksu
	virtual IOutputPin * outputPin(size_type idx) = 0;

	//! \return Ilość pinów wyjściowych
	virtual const size_type outputSize() const = 0;

	//! \return Ilość faktycznie podpiętych wyjść
	virtual const size_type outputsConnected() const = 0;
	//! \return Ilośc podłączonych pinów
	virtual const size_type connectedPins() const
	{
		return outputsConnected();
	}

private:

	//! Metoda wołana w momencie łączenia danego pinu
	//! \param pin Pin wyjściowy który jest właśnie podłanczany
	virtual void connectOutput(const IOutputPin * pin) = 0;
	//! Metoda wołana w momencie rozłanczenia danego pinu
	//! \param pin Pin wyjściowy który jest właśnie rozłanczany
	virtual void disconnectOutput(const IOutputPin * pin) = 0;
};

//! Interfejs węzła terminalnego (sink - odbiornika). Posiada on tylko piny wejściowe.
class ISinkNode : public virtual INode
{
friend class ModelBase;

public:

	//! \return Typ węzła
	virtual const NodeType type() const { return INode::SINK_NODE; }
	//! \param idx Indeks pinu wejsciowego
	//! \return Pin wejściowy spod wskazanego indeksu
	virtual const IInputPin * inputPin(size_type idx) const = 0;
	//! \param idx Indeks pinu wejsciowego
	//! \return Pin wejściowy spod wskazanego indeksu
	virtual IInputPin * inputPin(size_type idx) = 0;

	//! \return Ilość pinów wejściowych
	virtual const size_type inputSize() const = 0;

	//! \return Ilość faktycznie podpiętych wejść
	virtual const unsigned int inputsConnected() const = 0;
	//! \return Ilośc podłączonych pinów
	virtual const size_type connectedPins() const
	{
		return inputsConnected();
	}

private:
	//! Metoda wołana w momencie łączenia danego pinu
	//! \param pin Pin wejściowy który jest właśnie podłanczany
	virtual void connectInput(const IInputPin * pin) = 0;
	//! Metoda wołana w momencie rozłanczenia danego pinu
	//! \param pin Pin wyjściowy który jest właśnie rozłanczany
	virtual void disconnectInput(const IInputPin * pin) = 0;
};

//! Interfejs węzła przetwarzającego (procesor). Posiada piny wejściowe i wyjściowe.
class IProcessingNode : public ISinkNode, public ISourceNode
{
public:

	//! \return Typ węzła
	virtual const NodeType type() const { return INode::PROCESSING_NODE; }

	//! \return Ilośc podłączonych pinów
	virtual const size_type connectedPins() const
	{
		return inputsConnected() + outputsConnected();
	}
};

}

#endif	//	HEADER_GUARD___INODE_H__
