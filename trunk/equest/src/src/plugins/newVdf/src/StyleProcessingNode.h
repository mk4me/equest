/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleProcessingNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLEPROCESSINGNODE_H__
#define HEADER_GUARD_NEWVDF__STYLEPROCESSINGNODE_H__

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualProcessingNode.h>
#include <plugins/newVdf/INodeStrategy.h>

namespace vdf {

class StyleItem;
class IVisualInputPin;
class IVisualOutputPin;

class StyleProcessingNode : public IVisualProcessingNode
{
public:
    StyleProcessingNode();
    virtual ~StyleProcessingNode() {}

public:
    virtual void addInputPin( IVisualInputPinPtr pin );
    virtual void addOutputPin( IVisualOutputPinPtr pin );

    virtual void setName( const QString & name );
    virtual QGraphicsItem * visualItem() const;
    virtual void setSelection(bool selected);

	virtual int getNumInputPins() const { return static_cast<int>(inputPins.size()); }
	virtual IVisualInputPinPtr getInputPin(int no) { return inputPins[no]; }
	virtual int getNumOutputPins() const { return static_cast<int>(outputPins.size()); }
	virtual IVisualOutputPinPtr getOutputPin(int no) { return outputPins[no]; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

	virtual QString getName() const;

    virtual df::INode* getModelNode() {  return _node.get(); }
    virtual const df::INode* getModelNode() const { return _node.get(); }

    virtual const QIcon& getIcon() const { return icon; }
    virtual void setIcon(const QIcon& val) { icon = val; }

    virtual void setModelNode(df::INode* node) { _node.reset(node); }
    virtual Type getType() const { return IVisualItem::ProcessingNode; }
    virtual bool isType(Type t) const { return t == IVisualItem::ProcessingNode || t == IVisualItem::Node; }

    virtual void setValid( bool valid );

private:
    utils::shared_ptr<df::INode> _node;
    QIcon icon;
    StyleItem* styleItem;
    std::vector<IVisualInputPinPtr> inputPins;
    std::vector<IVisualOutputPinPtr> outputPins;
	INodeStrategyPtr strategy;
	QString name;
};

}

#endif
