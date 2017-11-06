#include "NewVdfPCH.h"
#include "StyleInputPinNode.h"
#include "StyleItem.h"


using namespace vdf;

StyleInputPinNode::StyleInputPinNode() :
    item(new StyleItem()),
	index(-1)
{
	item->setFlag(QGraphicsItem::ItemIsMovable, false);
}

StyleInputPinNode::~StyleInputPinNode()
{

}


void StyleInputPinNode::markRequired()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::markMissing()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::markNormal()
{
	strategy->setState(IPinStrategy::Normal);
	item->update();
}

void StyleInputPinNode::markCompatible()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::markIncompatible()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::markConnective()
{
	strategy->setState(IPinStrategy::Connective);
	item->update();
}

void StyleInputPinNode::markUnconnective()
{
	strategy->setState(IPinStrategy::Unconnective);
	item->update();
}

void StyleInputPinNode::markConnected()
{
	strategy->setState(IPinStrategy::Connected);
}

QGraphicsItem * StyleInputPinNode::visualItem() const
{
    return item;
}

void StyleInputPinNode::addSelection()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::removeSelection()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::addHover()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::removeHover()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::addCollision()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::removeCollision()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleInputPinNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	this->strategy = utils::dynamic_pointer_cast<IPinStrategy>(strategy);
	item->setStrategy(strategy);
}




df::IInputPin* vdf::StyleInputPinNode::getModelPin() const
{
    IVisualNodePtr parent = getParent().lock();
    UTILS_ASSERT(parent);

    df::INode* modelNode = parent->getModelNode();
    UTILS_ASSERT(modelNode && index >= 0);

    df::ISinkNode* sink = dynamic_cast<df::ISinkNode*>(modelNode);
    return sink->inputPin(index);
}

QPointF vdf::StyleInputPinNode::getConnectionPosHint()
{
    return visualItem()->scenePos() + strategy->getConnectionEndShift();
}
