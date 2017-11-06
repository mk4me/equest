#include <dfmlib/DFPin.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/Connection.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm {
////////////////////////////////////////////////////////////////////////////////

DFPin::DFPin(const std::string & nodeName, bool required,
	const Pin::ReqPinsSet & requiredPins)
	: Pin(nodeName, required, requiredPins),
		updated(false)
{

}

DFPin::~DFPin(void)
{

}

bool DFPin::isDFPin(const CPinPtr & pin)
{
	if(getDFPin(pin) != nullptr){
		return true;
	}

	return false;
}

DFPinPtr DFPin::getDFPin(const PinPtr & pin)
{
	return utils::dynamic_pointer_cast<DFPin>(pin);
}

CDFPinPtr DFPin::getDFPin(const CPinPtr & pin)
{
	return utils::dynamic_pointer_cast<const DFPin>(pin);
}

bool DFPin::isUpdated() const
{
	return updated;
}

void DFPin::reset()
{
	updated = false;
    onReset();
}

void DFPin::onReset()
{
    
}

void DFPin::update()
{
	updated = true;
    PinType pinType = getType();
	if(pinType == IN){
		DFNPtr node(DFNode::getDFNode(getParent()));
		if(node != nullptr){
			node->notify();
		}
	}else if(pinType == OUT){
		for(auto it = begin(); it != end(); ++it){
			DFPinPtr pin(utils::dynamic_pointer_cast<DFPin>((*it)->getDest()));
			if(pin != nullptr){
				pin->copyDataFromPin(utils::dynamic_pointer_cast<DFPin>(shared_from_this()));
			}
		}
	}

	onUpdate();
}

void DFPin::copyDataFromPin(const DFPinPtr & pin)
{

}

void DFPin::onUpdate()
{

}

}
