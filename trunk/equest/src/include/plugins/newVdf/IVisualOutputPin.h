/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:20
	filename: 	IVisualOutputPin.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALOUTPUTPIN_H__
#define HEADER_GUARD_NEWVDF__IVISUALOUTPUTPIN_H__

#include <plugins/newVdf/IVisualPin.h>

namespace df { class IOutputPin; }

namespace vdf {

class IVisualOutputPin : public IVisualPin
{
public:
	virtual Type getType() const { return IVisualItem::OutputPin; }
	virtual bool isType(Type t) const { return t == IVisualItem::OutputPin || t == IVisualItem::Pin; }
    virtual int getNumConnections() const = 0;
    virtual utils::weak_ptr<IVisualConnection> getConnection(int idx) const = 0;
    virtual void addConnection(utils::weak_ptr<IVisualConnection> connection) = 0;
    virtual void removeConnection(utils::weak_ptr<IVisualConnection> connection) = 0;
    virtual df::IOutputPin* getModelPin() const = 0;
};
DEFINE_SMART_POINTERS(IVisualOutputPin);
}

#endif
