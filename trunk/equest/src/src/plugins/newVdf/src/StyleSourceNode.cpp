#include "NewVdfPCH.h"
#include "StyleSourceNode.h"
#include <plugins/newVdf/INodeStrategy.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include "StyleItem.h"

using namespace vdf;

StyleSourceNode::StyleSourceNode() :
    styleItem(new StyleItem())
{

}

void StyleSourceNode::addOutputPin( IVisualOutputPinPtr pin )
{
    pin->visualItem()->setParentItem(styleItem);
    INodeStrategyPtr nodeStr = utils::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(static_cast<int>(pins.size()), false));
	pins.push_back(pin);
}


void StyleSourceNode::setName( const QString & name )
{
	this->name = name;
    styleItem->getStrategy()->update();
}

QGraphicsItem * StyleSourceNode::visualItem() const
{
    return styleItem;
}

void StyleSourceNode::setSelection(bool selected)
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

void StyleSourceNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	this->strategy = utils::dynamic_pointer_cast<INodeStrategy>(strategy);
	UTILS_ASSERT(this->strategy);
	styleItem->setStrategy(strategy);
}

QString StyleSourceNode::getName() const 
{
	return name;
}

void vdf::StyleSourceNode::setValid( bool valid )
{
    strategy->setState(valid ? INodeStrategy::Valid : INodeStrategy::Invalid);
}
