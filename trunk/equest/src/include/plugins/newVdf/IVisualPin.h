/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:35
    filename: IVisualPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALPIN_H__
#define HEADER_GUARD___IVISUALPIN_H__

//#include <dflib/Pin.h>
#include <plugins/newVdf/IVisualItem.h>

namespace vdf {

class IVisualNode;
class IVisualConnection;
class IVisualPin : public IVisualItem
{
protected:
	IVisualPin() {}

public:
	virtual ~IVisualPin() {

        int a = 0;
    }

	virtual void markNormal() = 0;
	virtual void markConnective() = 0;
	virtual void markUnconnective() = 0;
	virtual int getIndex() const = 0;
	virtual void setIndex(int val) = 0;
    virtual QPointF getConnectionPosHint() = 0;
	virtual utils::weak_ptr<IVisualNode> getParent() const = 0;
	virtual void setParent(utils::weak_ptr<IVisualNode> val) = 0;
};
DEFINE_SMART_POINTERS(IVisualPin);
}

#endif	//	HEADER_GUARD___IVISUALPIN_H__
