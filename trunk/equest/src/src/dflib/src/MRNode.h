/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   17:32
	filename: MRNode.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___MRNODE_H__
#define HEADER_GUARD___MRNODE_H__

#include "IMRNode.h"

#include <dflib/INode.h>
#include <dflib/IDFNode.h>
#include "MRNodeImpl.h"

class MRSinkNode : public IMRSinkNode
{
public:

	MRSinkNode(df::ISinkNode * node, df::IDFSink * sink);

	virtual ~MRSinkNode();

	virtual void process();

	virtual void pause();
	virtual void resume();
	virtual const bool paused() const;

	virtual void tryPause();

	virtual void updateSnk();

	virtual void reset();

	virtual void lockSnkProcessing();
	virtual void unlockSnkProcessing();

	virtual void addInputPin(MRInputPin * pin);

	virtual MRInputPin * inputPin(size_type idx);
	virtual const MRInputPin * inputPin(size_type idx) const;

	virtual const size_type inputSize() const;
	virtual const bool inputEmpty() const;

private:
	MRSinkNodeImpl * snkImpl;
	MRNodeImpl nodeImpl;
	df::ISinkNode * node_;
	df::IDFSink * sink_;
};

class MRSourceNode : public IMRSourceNode
{
public:

	MRSourceNode(df::ISourceNode * node, df::IDFSource * source);

	virtual ~MRSourceNode();

	virtual void process();

	virtual void pause();
	virtual void resume();
	virtual const bool paused() const;

	virtual void tryPause();

	virtual void updateSrc();

	virtual void reset();

	virtual void lockSrcProcessing();
	virtual void unlockSrcProcessing();

	virtual void addOutputPin(MROutputPin * pin);

	virtual MROutputPin * outputPin(size_type idx);
	virtual const MROutputPin * outputPin(size_type idx) const;

	virtual const size_type outputSize() const;
	virtual const bool outputEmpty() const;

private:
	MRSourceNodeImpl * srcImpl;
	MRNodeImpl nodeImpl;
	df::ISourceNode * node_;
	df::IDFSource * source_;
};

class MRProcessingNode : public IMRProcessingNode
{
public:

	MRProcessingNode(df::IProcessingNode * node, df::IDFProcessor * processor);
	virtual ~MRProcessingNode();

	virtual void process();

	virtual void pause();
	virtual void resume();
	virtual const bool paused() const;

	virtual void tryPause();

	virtual void updateSnk();
	virtual void updateSrc();

	virtual void reset();

	virtual void lockSnkProcessing();
	virtual void unlockSnkProcessing();

	virtual void addInputPin(MRInputPin * pin);

	virtual MRInputPin * inputPin(size_type idx);
	virtual const MRInputPin * inputPin(size_type idx) const;

	virtual const size_type inputSize() const;
	virtual const bool inputEmpty() const;

	virtual void lockSrcProcessing();
	virtual void unlockSrcProcessing();

	virtual void addOutputPin(MROutputPin * pin);

	virtual MROutputPin * outputPin(size_type idx);
	virtual const MROutputPin * outputPin(size_type idx) const;

	virtual const size_type outputSize() const;
	virtual const bool outputEmpty() const;

private:
	MRSinkNodeImpl * snkImpl;
	MRSourceNodeImpl * srcImpl;
	MRNodeImpl nodeImpl;
	df::IProcessingNode * node_;
	df::IDFProcessor * processor_;
};

#endif	//	HEADER_GUARD___MRNODE_H__
