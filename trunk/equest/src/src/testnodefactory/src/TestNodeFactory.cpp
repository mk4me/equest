#include <testnodefactory/TestNodeFactory.h>
#include <testnodefactory/TestDFPin.h>

#include <dfmlib/DFPin.h>

#include <sstream>
#include <vector>

TestNodeFactory::TestNodeFactory(void)
{
}


TestNodeFactory::~TestNodeFactory(void)
{
}

dflm::DFNPtr TestNodeFactory::createProcessingNode(const std::string & name, unsigned int inPins, unsigned int outPins, unsigned int requiredInPins, bool addRandomOutputDependecies){
	if(inPins == 0){
		return dflm::DFNPtr();
	}

	dflm::DFNPtr node(new TestDFNodeProcessing(name, inPins, requiredInPins, outPins, addRandomOutputDependecies));

	//unsigned int i = 0;
	//for(; i < inPins && i < requiredInPins; i++){
	//	std::stringstream name;
	//	name << i;
	//	node->addInPin(dflm::PinPtr(new TestDFPin("Input pin " + name.str() + " [REQUIRED]", true)));
	//}

	//for(; i < inPins; i++){
	//	std::stringstream name;
	//	name << i;
	//	node->addInPin(dflm::PinPtr(new TestDFPin("Input pin " + name.str())));
	//}

	//for(unsigned int i = 0; i < outPins; i++){
	//	std::stringstream name;
	//	name << i;
	//	std::set<dflm::WPinPtr> requiredPins;
	//	if(addRandomOutputDependecies == true){
	//		//generate random dependency
	//		std::vector<dflm::PinPtr> pins(node->getInPins().begin(), node->getInPins().end());
	//		std::random_shuffle(pins.begin(), pins.end());
	//		unsigned int count = rand() % pins.size();
	//		for(unsigned int i = 0; i < count; i++){
	//			requiredPins.insert(dflm::WPinPtr(pins[i]));
	//		}
	//	}
	//	node->addOutPin(dflm::PinPtr(new TestDFPin("Output pin " + name.str(), false, requiredPins)));
	//}

	return node;
}

dflm::DFSNPtr TestNodeFactory::createSourceNode(const std::string & name, unsigned int outPins){
	if(outPins == 0){
		return dflm::DFSNPtr();
	}
	
	
	dflm::DFSNPtr node(new TestDFNodeSource(name, outPins));

	/*for(unsigned int i = 0; i < outPins; i++){
		std::stringstream name;
		name << i;
		node->addOutPin(dflm::PinPtr(new TestDFPin("Output pin " + name.str())));
	}*/

	return node;
}
