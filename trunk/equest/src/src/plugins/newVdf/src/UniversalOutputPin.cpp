#include "NewVdfPCH.h"
#include <plugins/newVdf/UniversalOutputPin.h>

using namespace vdf;

void UniversalOutputPinBase::reset()
{
    val = ConstPtr();
}

void UniversalOutputPinBase::setWrapper( ConstPtr val )
{
    this->val = val;
}

const UniversalOutputPinBase::ConstPtr UniversalOutputPinBase::getWrapper() const
{
    return val;
}

UniversalOutputPinBase::UniversalOutputPinBase( df::ISourceNode * node ) : 
df::OutputPin(node)
{

}

utils::TypeInfo UniversalOutputPin::getInfo() const
{
    return info;
}

UniversalOutputPin::~UniversalOutputPin()
{

}

UniversalOutputPin::UniversalOutputPin( df::ISourceNode* node, const utils::TypeInfo& info ) : 
UniversalOutputPinBase(node),
    info(info)
{

}
