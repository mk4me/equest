#ifndef HEADER_GUARD__NODE_H__
#define HEADER_GUARD__NODE_H__

#include <dflib/IPin.h>
#include <algorithm>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

//! Wzorzec klasy realizującej przechowywanie pinów i śledzenie ilości podłączonych pinów
template<class Pin>
class NodeTImpl
{
protected:

	typedef std::vector<Pin*> Pins;

public:

	typedef typename Pins::size_type size_type;

public:

	NodeTImpl() : connectedPins_(0) {}
	NodeTImpl(size_type connected) : connectedPins_(connected) {}

	const size_type connectedPins() const
	{
		return connectedPins_;
	}

	const bool connected() const
	{
		return connectedPins() > 0;
	}

	void connect(const Pin * pin)
	{
		if(pin->connected() == false){
			++connectedPins_;
		}
	}

	void disconnect(const Pin * pin)
	{
		if(pin->connected() == false){
			--connectedPins_;
		}
	}

	const size_type size() const
	{
		return pins_.size();
	}

	const bool empty() const
	{
		return pins_.empty();
	}

	//! \param idx Indeks pinu wyjsciowego, zgłasza wyjątek jeśli niepoprawny index
	//! \return Pin wyjściowy spod wskazanego indeksu
	const Pin * pin(size_type idx) const
	{
		return pins_[idx];
	}

	Pin * pin(size_type idx)
	{
		return pins_[idx];
	}

	void addPin(Pin * pin)
	{
		auto it = find(pin);
		if(it != pins_.end())
		{
			throw std::runtime_error("Pin already managed by node");
		}

		pins_.push_back(pin);
	}

	void removePin(Pin * pin)
	{
		auto it = find(pin);
		if(it == pins_.end())
		{
			throw std::runtime_error("Pin not managed by node");
		}

		pins_.erase(it);
	}

private:

	typename Pins::iterator find(Pin * pin)
	{
		return std::find(pins_.begin(), pins_.end(), pin);
	}

private:

	//ModelBase * model_;
	Pins pins_;
	size_type connectedPins_;
};

//! Klasa realizująca podłanczanie węzła do modelu
class NodeT_
{
public:

	NodeT_();

	ModelBase * model();

	const ModelBase * model() const;

	void setModel(ModelBase * model);

	void checkModelForEdition();

private:

	ModelBase * model_;
};

//! Implementacja węzła źródłowego
class SourceNode : public ISourceNode
{	
public:

	virtual const ModelBase * model() const;

	virtual ModelBase * model();

	virtual const size_type outputsConnected() const;

	virtual const size_type outputSize() const;

	virtual const bool outputEmpty() const;

	//! \param idx Indeks pinu wyjsciowego, zgłasza wyjątek jeśli niepoprawny index
	//! \return Pin wyjściowy spod wskazanego indeksu
	virtual const IOutputPin * outputPin(size_type idx) const;

	virtual IOutputPin * outputPin(size_type idx);

private:

	virtual void connectOutput(const IOutputPin * pin);

	virtual void disconnectOutput(const IOutputPin * pin);

protected:

	void addOutputPin(IOutputPin * pin);

	void removeOutputPin(IOutputPin * pin);

private:

	virtual void setModel(ModelBase * model);

private:

	NodeTImpl<IOutputPin> nodeImpl;
	NodeT_ node_;
};


//! Implementacja węzła sinka
class SinkNode : public ISinkNode
{	
public:

	virtual const ModelBase * model() const;

	virtual ModelBase * model();

	virtual const size_type inputsConnected() const;

	virtual const size_type inputSize() const;

	virtual const bool inputEmpty() const;

	//! \param idx Indeks pinu wyjsciowego, zgłasza wyjątek jeśli niepoprawny index
	//! \return Pin wyjściowy spod wskazanego indeksu
	virtual const IInputPin * inputPin(size_type idx) const;

	virtual IInputPin * inputPin(size_type idx);

private:

	virtual void connectInput(const IInputPin * pin);

	virtual void disconnectInput(const IInputPin * pin);

protected:

	void addInputPin(IInputPin * pin);

	void removeInputPin(IInputPin * pin);

private:

	virtual void setModel(ModelBase * model);

private:

	NodeTImpl<IInputPin> nodeImpl;
	NodeT_ node_;
};

//! Implementacja węzła przetwarzającego
class ProcessingNode : public IProcessingNode
{
public:

	virtual const ModelBase * model() const;

	virtual ModelBase * model();

	virtual const size_type outputsConnected() const;

	virtual const size_type inputsConnected() const;

	virtual const size_type inputSize() const;

	virtual const bool inputEmpty() const;

	virtual const size_type outputSize() const;

	virtual const bool outputEmpty() const;

	//! \param idx Indeks pinu wyjsciowego, zgłasza wyjątek jeśli niepoprawny index
	//! \return Pin wyjściowy spod wskazanego indeksu
	virtual const IInputPin * inputPin(size_type idx) const;

	virtual IInputPin * inputPin(size_type idx);

	virtual const IOutputPin * outputPin(size_type idx) const;

	virtual IOutputPin * outputPin(size_type idx);

private:

	virtual void connectInput(const IInputPin * pin);

	virtual void disconnectInput(const IInputPin * pin);

	virtual void connectOutput(const IOutputPin * pin);

	virtual void disconnectOutput(const IOutputPin * pin);

protected:

	void addOutputPin(IOutputPin * pin);

	void removeOutputPin(IOutputPin * pin);

	void addInputPin(IInputPin * pin);

	void removeInputPin(IInputPin * pin);

private:

	virtual void setModel(ModelBase * model);

private:

	NodeTImpl<IInputPin> nodeIImpl;
	NodeTImpl<IOutputPin> nodeOImpl;
	NodeT_ node_;
};

}

#endif
