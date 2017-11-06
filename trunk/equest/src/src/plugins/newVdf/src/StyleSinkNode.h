/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleSinkNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLESINKNODE_H__
#define HEADER_GUARD_NEWVDF__STYLESINKNODE_H__

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualSinkNode.h>
#include <plugins/newVdf/INodeStrategy.h>

namespace vdf {

class StyleItem;
class IVisualInputPin;

class StyleSinkNode : public IVisualSinkNode
{
public:
    StyleSinkNode();
    virtual ~StyleSinkNode() {}

public:
    virtual void addInputPin( IVisualInputPinPtr pin );
    virtual void setName( const QString & name );
	virtual QString getName() const;
    virtual QGraphicsItem * visualItem() const;
    virtual void setSelection(bool selected);

	virtual int getNumInputPins() const { return static_cast<int>(pins.size()); }
	virtual IVisualInputPinPtr getInputPin(int no) { return pins[no]; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

    virtual df::INode* getModelNode() { return _node.get(); }
    virtual const df::INode* getModelNode() const { return _node.get(); }

    virtual const QIcon& getIcon() const { return icon; }
    virtual void setIcon(const QIcon& val) { icon = val; }

    virtual void setModelNode(df::INode* node) { _node.reset(node); }

    virtual Type getType() const { return IVisualItem::SinkNode; }
    virtual bool isType(Type t) const { return t == IVisualItem::SinkNode || t == IVisualItem::Node; }

    virtual void setValid( bool valid );


 private:
 	utils::shared_ptr<df::INode> _node;
    QIcon icon;
    StyleItem* styleItem;
    std::vector<IVisualInputPinPtr> pins;
	INodeStrategyPtr strategy;
	QString name;
};

}

#endif
