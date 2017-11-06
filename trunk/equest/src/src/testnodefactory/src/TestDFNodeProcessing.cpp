#include <testnodefactory/TestDFNodeProcessing.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include <testnodefactory/TestDFPin.h>

TestDFNodeProcessing::TestDFNodeProcessing(const std::string & nodeName, int inPins, int inRequired, int outPins, bool dependant)
    : DFNode(nodeName), inPins(inPins), inRequired(inRequired), outPins(outPins), dependant(dependant)
{
}


TestDFNodeProcessing::~TestDFNodeProcessing(void)
{
}

void TestDFNodeProcessing::process(){

    std::cout << getName().c_str() << " active - DATA PROCESSING procedure" << std::endl;

    if(checkInputPins() == true){
        std::cout << "inpute pins verified correctly - data on all connections avaiable" << std::endl;
        std::cout << "getting input data" << std::endl;

        if(isProcessingAllowed() == true){
			std::cout << "data processing allowed" << std::endl;
			std::cout << "processing data" << std::endl;
			processData();
		}

		if(isPropagatingAllowed() == true){
			std::cout << "data propagation allowed" << std::endl;
			std::cout << "propagating data" << std::endl;
			std::cout << "setting output pins data" << std::endl;
			setOutputData();
			std::cout << "updating output pins" << std::endl;
			updateOutputPins();
		}
	}
}

void TestDFNodeProcessing::doInitialization(const dflm::Node::PinsAdderPtr & pinsAdder)
{
    unsigned int i = 0;
    for(; i < inPins && i < inRequired; ++i){
        std::stringstream name;
        name << i;
        pinsAdder->addInPin(dflm::PinPtr(new TestDFPin("Input pin " + name.str() + " [REQUIRED]", true)));
    }

    for(; i < inPins; ++i){
        std::stringstream name;
        name << i;
        pinsAdder->addInPin(dflm::PinPtr(new TestDFPin("Input pin " + name.str())));
    }

    for(unsigned int i = 0; i < outPins; ++i){
        std::stringstream name;
        name << i;
        std::set<dflm::WPinPtr> requiredPins;
        if(dependant == true){
            //generate random dependency
            dflm::Node::Pins pins(beginIn(), endIn());
            std::random_shuffle(pins.begin(), pins.end());
            unsigned int count = rand() % pins.size();
            for(unsigned int i = 0; i < count; ++i){
                requiredPins.insert(dflm::WPinPtr(pins[i]));
            }
        }
        pinsAdder->addOutPin(dflm::PinPtr(new TestDFPin("Output pin " + name.str(), false, requiredPins)));
    }
}
