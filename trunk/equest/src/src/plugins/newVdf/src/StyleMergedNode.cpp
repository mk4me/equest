#include "NewVdfPCH.h"
#include "StyleMergedNode.h"
//#include "SimpleItem.h"

using namespace vdf;

MergedInputPin::MergedInputPin(IVisualInputPinPtr inputPin) :
	input(inputPin)
{

}

df::IInputPin* MergedInputPin::getModelPin() const
{
	return input->getModelPin();
}

