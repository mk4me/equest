#include "NewVdfPCH.h"
#include "StyleSinkNode.h"
#include "StyleItem.h"

using namespace vdf;

StyleSinkNode::StyleSinkNode() : 
	styleItem(new StyleItem)
{

}


void StyleSinkNode::addInputPin( IVisualInputPinPtr pin )
{
    pin->visualItem()->setParentItem(styleItem);
	INodeStrategyPtr nodeStr = utils::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(static_cast<int>(pins.size()), true));
    pins.push_back(pin);
}


void StyleSinkNode::setName( const QString & name )
{
	this->name = name;
    styleItem->getStrategy()->update();
}

QGraphicsItem * StyleSinkNode::visualItem() const
{
    return styleItem;
}

void StyleSinkNode::setSelection(bool selected)
{
    styleItem->setSelected(selected);
    if (selected) {
        strategy->setState(INodeStrategy::Selected);
        styleItem->setZValue(Z<IVisualItem::Node, true>::value());
    } else {
        strategy->setState(INodeStrategy::Normal);
        styleItem->setZValue(Z<IVisualItem::Node, false>::value());
    }
}



void StyleSinkNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	this->strategy = utils::dynamic_pointer_cast<INodeStrategy>(strategy);
	styleItem->setStrategy(strategy);
}

QString StyleSinkNode::getName() const
{
	return name;
}

void vdf::StyleSinkNode::setValid( bool valid )
{
    strategy->setState(valid ? INodeStrategy::Valid : INodeStrategy::Invalid);
}


