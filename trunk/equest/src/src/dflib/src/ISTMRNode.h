/********************************************************************
    created:  2012/12/10
    created:  10:12:2012   20:33
    filename: ISTMRNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTMRNODE_H__
#define HEADER_GUARD___ISTMRNODE_H__

#include <dflib/IDFFeatures.h>

class STMROutputPin;
class STMRInputPin;

class ISTMRNode : public df::IDFResetable
{
public:
	virtual bool hasSomethingToProcess() = 0;
	virtual void process() = 0;
};

class ISTMRSink
{
public:
	virtual void updateSnk() = 0;
	virtual ~ISTMRSink() {}
};

class ISTMRSource
{
public:
	virtual ~ISTMRSource() {}
};

class ISTMRSourceNode : public ISTMRSource, public virtual ISTMRNode
{
public:

	typedef unsigned int size_type;

public:

	virtual STMROutputPin * outputPin(size_type idx) = 0;
	virtual const STMROutputPin * outputPin(size_type idx) const = 0;

	virtual void addOutputPin(STMROutputPin * pin) = 0;

	virtual const size_type outputSize() const = 0;
	virtual const bool outputEmpty() const = 0;
};

class ISTMRSinkNode : public ISTMRSink, public virtual ISTMRNode
{
public:

	typedef unsigned int size_type;

public:

	virtual STMRInputPin * inputPin(size_type idx) = 0;
	virtual const STMRInputPin * inputPin(size_type idx) const = 0;

	virtual void addInputPin(STMRInputPin * pin) = 0;

	virtual const size_type inputSize() const = 0;
	virtual const bool inputEmpty() const = 0;
};

class ISTMRProcessingNode : public ISTMRSinkNode, public ISTMRSourceNode
{
public:

	typedef unsigned int size_type;
};

#endif	//	HEADER_GUARD___ISTMRNODE_H__
