#include <testnodefactory/TestDFNodeSource.h>


TestDFNodeSource::TestDFNodeSource(const std::string & nodeName, int outPins) : DFNode(nodeName), DFSourceNode(nodeName), TestDFNodeProcessing(nodeName, 0, 0, outPins, false), availableData(0)
{
}


TestDFNodeSource::~TestDFNodeSource(void)
{
}

//void TestDFNodeSource::addInPin(const dflm::PinPtr & inPin){
//	DFSourceNode::addInPin(inPin);
//}

bool TestDFNodeSource::checkInputPins() const{
	return true;
}

bool TestDFNodeSource::hasMoreData() const{
	if(availableData < 10){
		return true;
	}

	return false;
}

void TestDFNodeSource::doInitialization(const dflm::Node::PinsAdderPtr & pinsAdder)
{
    TestDFNodeProcessing::doInitialization(pinsAdder);
}

void TestDFNodeSource::process(){
	TestDFNodeProcessing::process();
	++availableData;
}
