/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   17:32
	filename: MRNode.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___STMRNODE_H__
#define HEADER_GUARD___STMRNODE_H__

#include "ISTMRNode.h"

#include <dflib/INode.h>
#include <dflib/IDFNode.h>
#include "STMRNodeImpl.h"

class STMRSinkNode : public ISTMRSinkNode
{
public:

	STMRSinkNode(df::ISinkNode * node, df::IDFSink * sink);

	virtual ~STMRSinkNode();

	virtual void process();

	
	virtual void reset();

	
	virtual void addInputPin(STMRInputPin * pin);

	virtual STMRInputPin * inputPin(size_type idx);
	virtual const STMRInputPin * inputPin(size_type idx) const;

	virtual const size_type inputSize() const;
	virtual const bool inputEmpty() const;

	virtual bool hasSomethingToProcess();

	virtual void updateSnk();

private:
	STMRSinkNodeImpl * snkImpl;
	STMRNodeImpl nodeImpl;
	df::ISinkNode * node_;
	df::IDFSink * sink_;
};

class STMRSourceNode : public ISTMRSourceNode
{
public:

	STMRSourceNode(df::ISourceNode * node, df::IDFSource * source);

	virtual ~STMRSourceNode();

	virtual void process();


	
	virtual void reset();

	virtual void addOutputPin(STMROutputPin * pin);

	virtual STMROutputPin * outputPin(size_type idx);
	virtual const STMROutputPin * outputPin(size_type idx) const;

	virtual const size_type outputSize() const;
	virtual const bool outputEmpty() const;

	virtual bool hasSomethingToProcess();

private:
	STMRSourceNodeImpl * srcImpl;
	STMRNodeImpl nodeImpl;
	df::ISourceNode * node_;
	df::IDFSource * source_;
};

class STMRProcessingNode : public ISTMRProcessingNode
{
public:

	STMRProcessingNode(df::IProcessingNode * node, df::IDFProcessor * processor);
	virtual ~STMRProcessingNode();

	virtual void process();


	
	virtual void reset();

	
	virtual void addInputPin(STMRInputPin * pin);

	virtual STMRInputPin * inputPin(size_type idx);
	virtual const STMRInputPin * inputPin(size_type idx) const;

	virtual const size_type inputSize() const;
	virtual const bool inputEmpty() const;

	
	virtual void addOutputPin(STMROutputPin * pin);

	virtual STMROutputPin * outputPin(size_type idx);
	virtual const STMROutputPin * outputPin(size_type idx) const;

	virtual const size_type outputSize() const;
	virtual const bool outputEmpty() const;

	virtual bool hasSomethingToProcess();

	virtual void updateSnk();

private:
	STMRSinkNodeImpl * snkImpl;
	STMRSourceNodeImpl * srcImpl;
	STMRNodeImpl nodeImpl;
	df::IProcessingNode * node_;
	df::IDFProcessor * processor_;
};

#endif	//	HEADER_GUARD___MRNODE_H__
