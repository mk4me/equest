/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   11:44
	filename: 	SimpleProcessingNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLEPROCESSINGNODE_H__
#define HEADER_GUARD_NEWVDF__SIMPLEPROCESSINGNODE_H__

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualProcessingNode.h>

namespace vdf {

class SimpleTextItem;
class IVisualInputPin;
class IVisualOutputPin;

class SimpleProcessingNode : public IVisualProcessingNode
{
public:
    SimpleProcessingNode();
    virtual ~SimpleProcessingNode() {}

public:
    virtual void addInputPin( IVisualInputPinPtr pin );
    virtual void removeInputPin( IVisualInputPinPtr pin );
    virtual void clearInputPins();
    virtual void addOutputPin( IVisualOutputPinPtr pin );
    virtual void removeOutputPin( IVisualOutputPinPtr pin );
    virtual void clearOutputPins();

    virtual void setName( const QString & name );
	virtual QString getName() const;
    virtual void setConfigButton( QAbstractButton * button );
    virtual void setCloseButton( QAbstractButton * button );
    virtual QGraphicsItem * visualItem() const;
    virtual void setSelection(bool selected);

	virtual int getNumInputPins() const { return static_cast<int>(inputPins.size()); }
	virtual IVisualInputPinPtr getInputPin(int no) { return inputPins[no]; }
	virtual int getNumOutputPins() const { return static_cast<int>(outputPins.size()); }
	virtual IVisualOutputPinPtr getOutputPin(int no) { return outputPins[no]; }
	virtual void setVisualStrategy( IVisualStrategyPtr strategy )
	{
		throw std::runtime_error("The method or operation is not implemented.");
	}
    virtual df::INode* getModelNode() { return _node.get(); }
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
    SimpleTextItem* simpleItem;
    std::vector<IVisualInputPinPtr> inputPins;
    std::vector<IVisualOutputPinPtr> outputPins;
	QString name;
};

}

#endif
