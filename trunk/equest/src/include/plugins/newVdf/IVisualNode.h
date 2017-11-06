/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:48
    filename: IVisualNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALNODE_H__
#define HEADER_GUARD___IVISUALNODE_H__

#include <plugins/newVdf/IVisualPin.h>
#include <QtWidgets/QAbstractButton>
#include <utils/Debug.h>
#include <dflib/INode.h>

namespace vdf {

class IVisualNode : public IVisualItem
{
protected:
	IVisualNode() {}

public:
	virtual ~IVisualNode() {  }

public:
	virtual void setName(const QString & name) = 0;
	virtual QString getName() const = 0;

    virtual void setSelection(bool selected) = 0;
    virtual void setValid(bool valid) = 0;

    virtual df::INode* getModelNode() = 0; 
    virtual const df::INode* getModelNode() const = 0;
    virtual void setModelNode(df::INode* node) = 0; 
    
    virtual const QIcon& getIcon() const = 0; 
    virtual void setIcon(const QIcon& val) = 0; 
};
DEFINE_SMART_POINTERS(IVisualNode);

}

#endif	//	HEADER_GUARD___IVISUALNODE_H__
