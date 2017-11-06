/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:20
	filename: 	IVisualInputPin.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALINPUTPIN_H__
#define HEADER_GUARD_NEWVDF__IVISUALINPUTPIN_H__

#include <plugins/newVdf/IVisualPin.h>

namespace df { class IInputPin; }

namespace vdf {

class IVisualInputPin : public IVisualPin
{
public:
	virtual void markRequired() = 0;
	virtual void markMissing() = 0;
	virtual Type getType() const { return IVisualItem::InputPin; }
	virtual bool isType(Type t) const { return t == IVisualItem::InputPin || t == IVisualItem::Pin; }
    virtual utils::weak_ptr<IVisualConnection> getConnection() const = 0;
    virtual void setConnection(utils::weak_ptr<IVisualConnection> connection) = 0;
    virtual df::IInputPin* getModelPin() const = 0;
};
DEFINE_SMART_POINTERS(IVisualInputPin);
}

#endif
