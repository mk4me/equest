/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:31
	filename: 	StyleInputPinNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLEINPUTPINNODE_H__
#define HEADER_GUARD_NEWVDF__STYLEINPUTPINNODE_H__

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IPinStrategy.h>
#include <plugins/newVdf/IVisualConnection.h>
#include <plugins/newVdf/IVisualNode.h>

namespace vdf {

class StyleItem;

class StyleInputPinNode : public IVisualInputPin
{
public:
    StyleInputPinNode();
    virtual ~StyleInputPinNode();

public:
    virtual void markRequired();
    virtual void markMissing();
    virtual void markNormal();
    virtual void markCompatible();
    virtual void markIncompatible();
    virtual void markConnective();
    virtual void markUnconnective();
    virtual void markConnected();
    virtual QGraphicsItem * visualItem() const;
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();
	virtual int getIndex() const { UTILS_ASSERT(index != -1); return index; }
	virtual void setIndex(int val) { index = val; }
	IVisualNodeWeakPtr getParent() const { return parent; }
	void setParent(IVisualNodeWeakPtr val) { parent = val; }
	virtual void setConnection(IVisualConnectionWeakPtr connection) { this->connection = connection; }
	virtual IVisualConnectionWeakPtr getConnection() const { return connection; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

    virtual df::IInputPin* getModelPin() const;
    virtual QPointF getConnectionPosHint();
protected:
	IVisualNodeWeakPtr parent;
	IVisualConnectionWeakPtr connection;
	IPinStrategyPtr strategy;
	int index;
    StyleItem* item;
};

}

#endif
