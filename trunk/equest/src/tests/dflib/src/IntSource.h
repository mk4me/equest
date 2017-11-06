#ifndef HEADER_GUARD__INT_SOURCE_H__
#define HEADER_GUARD__INT_SOURCE_H__

#include "IntPin.h"
#include <dflib/IDFNode.h>

class IntSource : public df::SourceNode, public df::IDFSource
{
public:

	typedef std::vector<int> Data;

public:

	IntSource(const Data & data, const std::string & name = "") : data(data), idx(0), name(name)
	{
		outPinA = new IntOutputPin(this);
		addOutputPin(outPinA);
	}

	virtual void reset() { idx = 0; }

	virtual const bool empty() const { return idx == data.size(); }
	
	virtual void produce()
	{
		outPinA->value(data[idx++]);
	}

private:
	std::string name;
	IntOutputPin * outPinA;
	Data data;
	int idx;
};

#endif // HEADER_GUARD__INT_SOURCE_H__