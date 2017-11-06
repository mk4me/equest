#ifndef HEADER_GUARD__DFM_TESTSOURCENODE_H__
#define HEADER_GUARD__DFM_TESTSOURCENODE_H__

#include <dfmlib/DFSourceNode.h>
#include <testnodefactory/TestDFNodeProcessing.h>

class TestDFNodeSource :
	public dflm::DFSourceNode, public TestDFNodeProcessing
{
public:
	friend class TestNodeFactory;

public:
	virtual ~TestDFNodeSource(void);
	virtual bool hasMoreData() const;

	//virtual void addInPin(dflm::PinPtr inPin);

protected:
	TestDFNodeSource(const std::string & nodeName, int outPins);

	virtual bool checkInputPins() const;
	virtual void process();

    virtual void doInitialization(const PinsAdderPtr & pinsAdder);

private:

    /*virtual void addInPin(const PinPtr & pin);*/

protected:
	unsigned int availableData;
};

#endif
