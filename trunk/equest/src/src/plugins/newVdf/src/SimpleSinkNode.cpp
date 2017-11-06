#include "NewVdfPCH.h"
#include "SimpleSinkNode.h"
#include "SimpleItem.h"

using namespace vdf;

SimpleSinkNode::SimpleSinkNode() : 
    simpleItem(new SimpleTextItem (0,0))
{
}


void SimpleSinkNode::addInputPin( IVisualInputPinPtr pin )
{
    pin->visualItem()->setParentItem(simpleItem);
    pin->visualItem()->setPos(margin, margin + pins.size() * pinHeight);
    pins.push_back(pin);
	simpleItem->setSize(calculateNodeSize(static_cast<int>(pins.size())));
}

void SimpleSinkNode::setName( const QString & name )
{
	this->name = name;
    simpleItem->setText(name);
}

QGraphicsItem * SimpleSinkNode::visualItem() const
{
    return simpleItem;
}

void SimpleSinkNode::setSelection(bool selected)
{
    simpleItem->setSelected(selected);
}



void SimpleSinkNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	throw std::runtime_error("The method or operation is not implemented.");
}

QString SimpleSinkNode::getName() const
{
	return name;
}

void vdf::SimpleSinkNode::setValid( bool valid )
{

}


