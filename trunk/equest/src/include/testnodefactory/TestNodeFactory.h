#ifndef HEADER_GUARD__DFM_TESTNODEFACTORY_H__
#define HEADER_GUARD__DFM_TESTNODEFACTORY_H__

#include <testnodefactory/TestDFNodeProcessing.h>
#include <testnodefactory/TestDFNodeSource.h>

class TestNodeFactory
{
public:
	~TestNodeFactory(void);

	static dflm::DFNPtr createProcessingNode(const std::string & name, unsigned int inPins, unsigned int outPins, unsigned int requiredInPins = 0, bool addRandomOutputDependecies = false);
	static dflm::DFSNPtr createSourceNode(const std::string & name, unsigned int outPins);

private:
	TestNodeFactory(void);
};

#endif
