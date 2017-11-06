/********************************************************************
    created:  2012/12/10
    created:  10:12:2012   20:33
    filename: IMRNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IMRNODE_H__
#define HEADER_GUARD___IMRNODE_H__

#include <dflib/IDFFeatures.h>
#include <mutex>

class MROutputPin;
class MRInputPin;

class IMRNode : public df::IDFResetable, public df::IDFPausable
{
public:
	virtual void process() = 0;
	virtual void tryPause() = 0;

protected:

	mutable std::mutex mutex;
};

class IMRSink
{
public:
	virtual ~IMRSink() {}
	virtual void updateSnk() = 0;
};

class IMRSource
{
public:
	virtual ~IMRSource() {}
	virtual void updateSrc() = 0;
};

class IMRSourceNode : public IMRSource, public virtual IMRNode
{
public:

	typedef unsigned int size_type;

public:
	virtual void lockSrcProcessing() = 0;
	virtual void unlockSrcProcessing() = 0;

	virtual MROutputPin * outputPin(size_type idx) = 0;
	virtual const MROutputPin * outputPin(size_type idx) const = 0;

	virtual void addOutputPin(MROutputPin * pin) = 0;

	virtual const size_type outputSize() const = 0;
	virtual const bool outputEmpty() const = 0;
};

class IMRSinkNode : public IMRSink, public virtual IMRNode
{
public:

	typedef unsigned int size_type;

public:
	virtual void lockSnkProcessing() = 0;
	virtual void unlockSnkProcessing() = 0;

	virtual MRInputPin * inputPin(size_type idx) = 0;
	virtual const MRInputPin * inputPin(size_type idx) const = 0;

	virtual void addInputPin(MRInputPin * pin) = 0;

	virtual const size_type inputSize() const = 0;
	virtual const bool inputEmpty() const = 0;
};

class IMRProcessingNode : public IMRSinkNode, public IMRSourceNode
{
public:

	typedef unsigned int size_type;
};

#endif	//	HEADER_GUARD___IMRNODE_H__
