/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:39
	filename: 	IVisualSourceNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALSOURCENODE_H__
#define HEADER_GUARD_NEWVDF__IVISUALSOURCENODE_H__

#include <plugins/newVdf/IVisualNode.h>
#include <plugins/newVdf/IVisualOutputPin.h>

namespace vdf {

class IVisualOutputPin;
class IVisualInputPin;
class IVisualSourceNode : public virtual IVisualNode
{
public:
    virtual void addOutputPin(IVisualOutputPinPtr pin) = 0;
	virtual int getNumOutputPins() const = 0;
	virtual IVisualOutputPinPtr getOutputPin(int no) = 0;
};
DEFINE_SMART_POINTERS(IVisualSourceNode);

}

#endif	
