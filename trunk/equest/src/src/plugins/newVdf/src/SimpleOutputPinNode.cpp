#include "NewVdfPCH.h"
#include "SimpleOutputPinNode.h"
#include "SimpleItem.h"
#include <dflib/INode.h>

using namespace vdf;

SimpleOutputPinNode::SimpleOutputPinNode() :
	item(new SimplePinItem()),
	index(-1)
{
}

SimpleOutputPinNode::~SimpleOutputPinNode()
{
}


void SimpleOutputPinNode::markNormal()
{
	item->setColor(QColor(180, 180, 180));
	item->update();
}

void SimpleOutputPinNode::markConnective()
{
	item->setColor(QColor(50, 50, 200));
	item->update();
}

void SimpleOutputPinNode::markUnconnective()
{
	item->setColor(QColor(230, 0, 0));
	item->update();
}

QGraphicsItem * SimpleOutputPinNode::visualItem() const
{
    return item;
}

void SimpleOutputPinNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	throw std::runtime_error("The method or operation is not implemented.");
}

void vdf::SimpleOutputPinNode::removeConnection( utils::weak_ptr<IVisualConnection> connection )
{
    auto c = connection.lock();
    auto it = std::find_if(connections.begin(), connections.end(), 
        [&](const IVisualConnectionWeakPtr& val)
        { 
            return val.lock() == c; 
        }
    );
    if (it != connections.end()) {
        connections.erase(it);
    }
}

df::IOutputPin* vdf::SimpleOutputPinNode::getModelPin() const
{
    IVisualNodePtr parent = getParent().lock();
    UTILS_ASSERT(parent);

    df::INode* modelNode = parent->getModelNode();
    UTILS_ASSERT(modelNode && index >= 0);

    df::ISourceNode* source = dynamic_cast<df::ISourceNode*>(modelNode);
    return source->outputPin(index);
}

QPointF vdf::SimpleOutputPinNode::getConnectionPosHint()
{
    return visualItem()->scenePos();
}
