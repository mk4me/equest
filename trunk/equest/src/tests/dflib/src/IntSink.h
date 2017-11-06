#ifndef HEADER_GUARD__INT_SINK_H__
#define HEADER_GUARD__INT_SINK_H__

#include "IntPin.h"
#include <dflib/IDFNode.h>

class IntSink : public df::SinkNode, public df::IDFSink
{
public:
	typedef std::vector<int> Data;

public:

	IntSink(const std::string & name = "")
	{
		inPinA = new IntInputPin(this);
		addInputPin(inPinA);
	}

	virtual void reset() { Data().swap(data_); }

	virtual void consume()
	{
		data_.push_back(inPinA->value());
	}

	const Data & data() const
	{
		return data_;
	}

private:
	std::string name;
	IntInputPin * inPinA;
	Data data_;
};

#endif // HEADER_GUARD__INT_SINK_H__