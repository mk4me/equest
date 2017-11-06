#include <dfmlib/DFInterface.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFInterface::DFInterface(bool enable) : objectEnable(enable)
{

}


DFInterface::~DFInterface(void)
{

}

void DFInterface::setEnable(bool enable)
{
    if(objectEnable == enable){
        return;
    }

    objectEnable = enable;

    notify();
}

bool DFInterface::isEnable() const
{
	return objectEnable;
}

void DFInterface::reset()
{

}

void DFInterface::notify()
{
	if(objectEnable == true){
		process();
	}
}

}
