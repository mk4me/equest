#ifndef HEADER_GUARD__INT_PIN_H__
#define HEADER_GUARD__INT_PIN_H__

#include <dflib/Pin.h>
#include <dflib/IDFPin.h>


class IntOutputPin : public df::OutputPin, public df::IDFOutput
{
public:
	IntOutputPin(df::ISourceNode * node) : df::OutputPin(node), val(0) {}

	const int value() const { return val; }
	void value(int val) { this->val = val; }

	virtual void reset()
	{
		val = 0;
	}

private:
	int val;
};


class IntInputPin : public df::InputPin, public df::IDFInput
{
public:

	IntInputPin(df::ISinkNode * node) : df::InputPin(node), val(0) {}

	virtual void copyData(const df::IDFOutput * pin)
	{
		val = dynamic_cast<const IntOutputPin*>(pin)->value();
	}

	virtual void reset()
	{
		val = 0;
	}

	const int value() const { return val; }

private:

	int val;
};



#endif // HEADER_GUARD__INT_PIN_H__