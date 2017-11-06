#include "NewVdfPCH.h"
#include "StyleProcessingNode.h"
#include <plugins/newVdf/INodeStrategy.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include "StyleItem.h"

using namespace vdf;

StyleProcessingNode::StyleProcessingNode() :
    styleItem(new StyleItem())
{

}

void StyleProcessingNode::addInputPin( IVisualInputPinPtr pin )
{
    pin->visualItem()->setParentItem(styleItem);

	INodeStrategyPtr nodeStr = utils::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(static_cast<int>(inputPins.size()), true));

    inputPins.push_back(pin);
}


void StyleProcessingNode::setName( const QString & name )
{
    this->name = name;
	this->styleItem->getStrategy()->update();
}


QGraphicsItem * StyleProcessingNode::visualItem() const
{
    return styleItem;
}

void StyleProcessingNode::setSelection(bool selected)
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


void StyleProcessingNode::addOutputPin( IVisualOutputPinPtr pin )
{
    pin->visualItem()->setParentItem(styleItem);

	INodeStrategyPtr nodeStr = utils::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(static_cast<int>(outputPins.size()), false));

    outputPins.push_back(pin);
}

void StyleProcessingNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	UTILS_ASSERT(styleItem && utils::dynamic_pointer_cast<INodeStrategy>(strategy));
	this->strategy = utils::dynamic_pointer_cast<INodeStrategy>(strategy);
	styleItem->setStrategy(this->strategy);
}

QString StyleProcessingNode::getName() const 
{
	return name;
}

void vdf::StyleProcessingNode::setValid( bool valid )
{
    strategy->setState(valid ? INodeStrategy::Valid : INodeStrategy::Invalid);
}

