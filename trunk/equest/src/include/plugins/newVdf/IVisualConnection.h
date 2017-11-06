/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:44
    filename: IVisualConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALCONNECTION_H__
#define HEADER_GUARD___IVISUALCONNECTION_H__

#include <dflib/IConnection.h>
#include <plugins/newVdf/IVisualItem.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <utils/Debug.h>

namespace df { class IConnection; }

namespace vdf {
    
class IVisualConnection : public IVisualItem
{
public:
	virtual ~IVisualConnection() {}

public:
	virtual void setInputPin(IVisualInputPinPtr pin) = 0;
	virtual void setOutputPin(IVisualOutputPinPtr pin) = 0;
	virtual IVisualInputPinPtr getInputPin() = 0;
	virtual IVisualOutputPinPtr getOutputPin() = 0;
	
    virtual df::IConnection* getModelConnection() = 0; 
	virtual const df::IConnection* getModelConnection() const = 0; 
	virtual void setModelConnection(df::IConnection* node) = 0; 

	virtual IVisualItem::Type getType() const = 0;
	virtual bool isType(IVisualItem::Type t) const = 0; 

    virtual bool isSelected() const = 0;
    virtual void setSelected(bool val) = 0;
};
DEFINE_SMART_POINTERS(IVisualConnection);

}

#endif	//	HEADER_GUARD___IVISUALCONNECTION_H__
