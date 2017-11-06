#include <dfmlib/DFNode.h>
#include <dfmlib/Pin.h>
#include <dfmlib/DFPin.h>
#include <dfmlib/Connection.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFNode::DFNode(const std::string & nodeName)/*, bool processingAllowed, bool propagatingAllowed)
	*/: Node(nodeName), processingAllowed(true),
	propagatingAllowed(true)
{

}


DFNode::~DFNode(void)
{

}

DFNPtr DFNode::getDFNode(const NPtr & node)
{
	return utils::dynamic_pointer_cast<DFNode>(node);
}

void DFNode::allowProcessing(bool allow)
{
    if(processingAllowed == allow){
        return;
    }

    processingAllowed = allow;
}

void DFNode::allowPropagating(bool allow)
{
    if(propagatingAllowed == allow){
        return;
    }

    propagatingAllowed = allow;
    if(allow == true){
        notify();
    }
}

bool DFNode::isProcessingAllowed() const
{
	return processingAllowed;
}

bool DFNode::isPropagatingAllowed() const
{
	return propagatingAllowed;
}


bool DFNode::checkInputPins() const
{
	for(auto it = beginIn(); it != endIn(); ++it){
		for(auto iT = (*it)->begin(); iT != (*it)->end(); ++iT){
			DFPinPtr ptr(DFPin::getDFPin((*iT)->getOther(*it)));
			if( ptr != nullptr && ptr->isUpdated() == false){
				return false;
			}
		}
	}

	return true;
}

void DFNode::updateOutputPins()
{
	for(auto it = beginOut(); it != endOut(); ++it){
		DFPinPtr pin(DFPin::getDFPin(*it));
		
		if(pin != nullptr){
			pin->update();		
            for(auto iT = (*it)->begin(); iT != (*it)->end(); ++iT){
				pin = DFPin::getDFPin((*iT)->getOther(*it));
				if(pin != nullptr){
					pin->update();
				}
			}
		}
	}
}

void DFNode::setOutputData()
{

}

void DFNode::processData()
{

}

void DFNode::process()
{
	if(checkInputPins() == true){

		if(processingAllowed == true){
			processData();
		}

		if(propagatingAllowed == true){	
			setOutputData();
			updateOutputPins();
		}

        if(onLeafProcessedCallback){
            onLeafProcessedCallback();
        }
	}
}

}
