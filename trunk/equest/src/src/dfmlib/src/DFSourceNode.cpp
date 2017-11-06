#include <dfmlib/DFSourceNode.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFSourceNode::DFSourceNode(const std::string & nodeName/*, bool processingAllowed,
	bool propagatingAllowed*/) : DFNode(nodeName/*,processingAllowed,propagatingAllowed*/)
{

}


DFSourceNode::~DFSourceNode(void)
{

}

void DFSourceNode::addInPin(const PinPtr & pin)
{
	throw std::runtime_error("Can not add input pin to source node!");
}

bool DFSourceNode::checkInputPins() const
{
	return true;
}

void DFSourceNode::resetInputPins()
{

}

}
