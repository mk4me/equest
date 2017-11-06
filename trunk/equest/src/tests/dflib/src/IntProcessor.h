#ifndef HEADER_GUARD__INT_PROCESSOR_H__
#define HEADER_GUARD__INT_PROCESSOR_H__

#include "IntPin.h"
#include <dflib/Node.h>
#include <dflib/IDFNode.h>
#include <boost/function.hpp>

class IntProcessor : public df::ProcessingNode, public df::IDFProcessor
{
public:
	typedef boost::function<int (int, int)> Func;

public:

	IntProcessor(const Func & func, const std::string & name = "") : func(func), name(name)
	{
		inPinA = new IntInputPin(this);
		inPinB = new IntInputPin(this);
		outPinA = new IntOutputPin(this);
		addInputPin(inPinA);
		addInputPin(inPinB);
		addOutputPin(outPinA);
	}

	virtual void reset() {}

	virtual void process()
	{
		outPinA->value(func(inPinA->value(), inPinB->value()));
	}

private:
	Func func;
	IntInputPin * inPinA;
	IntInputPin * inPinB;
	IntOutputPin * outPinA;

	std::string name;

};

#endif // HEADER_GUARD__INT_PROCESSOR_H__