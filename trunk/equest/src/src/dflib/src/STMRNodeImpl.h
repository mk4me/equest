/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   20:35
	filename: MRNodeImpl.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___STMRNODEIMPL_H__
#define HEADER_GUARD___STMRNODEIMPL_H__

#include <vector>

class STMRInputPin;
class STMROutputPin;
class ISTMRSourceNode;
class ISTMRSinkNode;

class STMRNodeImpl
{
public:

	STMRNodeImpl();

	void pause();
	void resume();

	const bool paused() const;

	void tryPause();

private:
	bool paused_;
};

class STMRSourceNodeImpl
{
private:

	typedef std::vector<STMROutputPin*> OutputPins;
	typedef OutputPins::size_type size_type;

public:

	STMRSourceNodeImpl(ISTMRSourceNode * source, unsigned int toConsume);

	~STMRSourceNodeImpl();

	void updateSrc();

	void updateOutputs();

	void resetOutputs();

	void addPin(STMROutputPin * pin);

	const bool empty() const;

	const size_type size() const;

	STMROutputPin * pin(size_type idx);
	const STMROutputPin * pin(size_type idx) const;

	void wait();
	void wakeUp();

private:	
	const unsigned int toConsume;
	unsigned int currentlyConsumed;
	ISTMRSourceNode * source_;
	OutputPins outputPins;
};

class STMRSinkNodeImpl
{
private:

	typedef std::vector<STMRInputPin*> InputPins;
	typedef InputPins::size_type size_type;

public:

	STMRSinkNodeImpl(ISTMRSinkNode * sink, unsigned int toConsume);

	~STMRSinkNodeImpl();

	void updateSnk();

	void consumeInputs();

	void resetInputs();

	void addPin(STMRInputPin * pin);

	const bool empty() const;

	const size_type size() const;

	STMRInputPin * pin(size_type idx);
	const STMRInputPin * pin(size_type idx) const;

	void wait();

	void wakeUp();

	bool hasSomethingToProcess() const;

private:
	const unsigned int toConsume;
	unsigned int readyToConsume;
	ISTMRSinkNode * sink_;
	InputPins inputPins;
};

#endif	//	HEADER_GUARD___MRNODEIMPL_H__
