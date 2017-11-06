/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   20:35
	filename: MRNodeImpl.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___MRNODEIMPL_H__
#define HEADER_GUARD___MRNODEIMPL_H__

#include <vector>
#include <mutex>
#include <condition_variable>

class MRInputPin;
class MROutputPin;
class IMRSourceNode;
class IMRSinkNode;

class MRNodeImpl
{
public:

	MRNodeImpl();

	void pause();
	void resume();

	const bool paused() const;

	void tryPause();

private:
	bool paused_;
	std::mutex syncPolicy;
};

class MRSourceNodeImpl
{
private:

	typedef std::vector<MROutputPin*> OutputPins;
	typedef OutputPins::size_type size_type;

public:

	MRSourceNodeImpl(IMRSourceNode * source, unsigned int toConsume);

	~MRSourceNodeImpl();

	void updateSrc();

	void updateOutputs();

	void resetOutputs();

	void addPin(MROutputPin * pin);

	const bool empty() const;

	const size_type size() const;

	MROutputPin * pin(size_type idx);
	const MROutputPin * pin(size_type idx) const;

	void wait();
	void wakeUp();

private:	
	std::mutex syncPolicy;
	std::condition_variable wait_;
	const unsigned int toConsume;
	unsigned int currentlyConsumed;
	IMRSourceNode * source_;
	OutputPins outputPins;
};

class MRSinkNodeImpl
{
private:

	typedef std::vector<MRInputPin*> InputPins;
	typedef InputPins::size_type size_type;

public:

	MRSinkNodeImpl(IMRSinkNode * sink, unsigned int toConsume);

	~MRSinkNodeImpl();

	void updateSnk();

	void consumeInputs();

	void resetInputs();

	void addPin(MRInputPin * pin);

	const bool empty() const;

	const size_type size() const;

	MRInputPin * pin(size_type idx);
	const MRInputPin * pin(size_type idx) const;

	void wait();

	void wakeUp();

private:
	std::condition_variable wait_;
	std::mutex sync;
	const unsigned int toConsume;
	unsigned int readyToConsume;
	IMRSinkNode * sink_;
	InputPins inputPins;
};

#endif	//	HEADER_GUARD___MRNODEIMPL_H__
