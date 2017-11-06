/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   12:20
	filename: 	SimpleSourceNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLESOURCENODE_H__
#define HEADER_GUARD_NEWVDF__SIMPLESOURCENODE_H__

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualSourceNode.h>

namespace vdf {

class SimpleTextItem;
class IVisualOutputPin;

class SimpleSourceNode : public IVisualSourceNode
{
public:
    SimpleSourceNode();
    virtual ~SimpleSourceNode() {}

public:
    virtual void addOutputPin( IVisualOutputPinPtr pin );
    virtual void removeOutputPin( IVisualOutputPinPtr pin );
    virtual void clearOutputPins();
    virtual void setName( const QString & name );
	virtual QString getName() const;
    virtual void setConfigButton( QAbstractButton * button );
    virtual void setCloseButton( QAbstractButton * button );
    virtual QGraphicsItem * visualItem() const;
    virtual void setSelection(bool selected);

	virtual int getNumOutputPins() const { return static_cast<int>(pins.size()); }
	virtual IVisualOutputPinPtr getOutputPin(int no) { return pins[no]; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

    virtual df::INode* getModelNode() { return _node.get(); }
    virtual const df::INode* getModelNode() const { return _node.get(); }

    virtual const QIcon& getIcon() const { return icon; }
    virtual void setIcon(const QIcon& val) { icon = val; }

    virtual void setModelNode(df::INode* node) { _node.reset(node); }
    virtual Type getType() const { return IVisualItem::SourceNode; }
    virtual bool isType(Type t) const { return t == IVisualItem::SourceNode || t == IVisualItem::Node; }

    virtual void setValid( bool valid );


private:
    utils::shared_ptr<df::INode> _node;
    QIcon icon;
    SimpleTextItem* simpleItem;
    std::vector<IVisualOutputPinPtr> pins;
	QString name;
};

}

#endif
