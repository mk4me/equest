#include "NewVdfPCH.h"
#include "SimpleMergedNode.h"
#include "StyleItem.h"


vdf::MergedOutputPin::MergedOutputPin( IVisualOutputPinPtr outputPin ) :
	output(outputPin)
{
}

df::IOutputPin* vdf::MergedOutputPin::getModelPin() const
{
	return output->getModelPin();
}