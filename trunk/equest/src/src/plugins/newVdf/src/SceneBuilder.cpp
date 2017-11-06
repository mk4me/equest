#include "NewVdfPCH.h"
#include "SceneBuilder.h"
#include <plugins/newVdf/IVisualItem.h>
#include <plugins/newVdf/IVisualProcessingNode.h>
#include <plugins/newVdf/IVisualSinkNode.h>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualOutputPin.h>

using namespace vdf;

SceneBuilder::VisualNodeWithPins SceneBuilder::createProcessing(const QString& text, const QIcon& icon, df::INode* prototype, int in, int out ) const
{
    IVisualProcessingNodePtr node = factories->getCurrentNodesFactory()->createProcessingNode();
    // hack! ikona nie jest doimplementowana i nie odswieza strategii (dopiero setName to robi)
    node->setIcon(icon);
    node->setName(text);
	node->setModelNode(prototype);
    auto input = addInputPins(node, in);
    auto output = addOutputPins(node, out);
	node->visualItem()->setZValue(Z<IVisualItem::Node, false>::value());
    return boost::make_tuple(node, input, output);
}

SceneBuilder::VisualNodeWithPins SceneBuilder::createSink(const QString& text, const QIcon& icon, df::INode* prototype, int in ) const
{
    IVisualSinkNodePtr node = factories->getCurrentNodesFactory()->createSinkNode();
    // hack! ikona nie jest doimplementowana i nie odswieza strategii (dopiero setName to robi)
    node->setIcon(icon);
    node->setName(text);
	node->setModelNode(prototype);
    auto input = addInputPins(node, in);
	node->visualItem()->setZValue(Z<IVisualItem::Node, false>::value());
    return boost::make_tuple(node, input, Pins());
}

SceneBuilder::VisualNodeWithPins SceneBuilder::createSource(const QString& text, const QIcon& icon, df::INode* prototype, int out ) const
{
    IVisualSourceNodePtr node = factories->getCurrentNodesFactory()->createSourceNode();
    // hack! ikona nie jest doimplementowana i nie odswieza strategii (dopiero setName to robi)
    node->setIcon(icon);
    node->setName(text);
	node->setModelNode(prototype);
    auto output = addOutputPins(node, out);
	node->visualItem()->setZValue(Z<IVisualItem::Node, false>::value());
    return boost::make_tuple(node, Pins(), output);
}

SceneBuilder::Pins SceneBuilder::addInputPins( IVisualSinkNodePtr sink, int count ) const
{
    Pins pins;
    for (int i = 0; i < count; ++i) {
        IVisualInputPinPtr pin = factories->getCurrentPinsFactory()->createInputPin();
        sink->addInputPin(pin);
		pin->setParent(sink);
        pin->visualItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
		pin->visualItem()->setZValue(Z<IVisualItem::Pin, false>::value());
		pin->setIndex(i);
        pins.push_back(pin);
    }
    return pins;
}

SceneBuilder::Pins SceneBuilder::addOutputPins( IVisualSourceNodePtr source, int count ) const
{
    Pins pins;
    for (int i = 0; i < count; ++i) {
        IVisualOutputPinPtr pin = factories->getCurrentPinsFactory()->createOutputPin();
        source->addOutputPin(pin);
		pin->setParent(source);
        pin->visualItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
		pin->visualItem()->setZValue(Z<IVisualItem::Pin, false>::value());
		pin->setIndex(i);
        pins.push_back(pin);
    }
    return pins;
}

SceneBuilder::VisualNodeWithPins  SceneBuilder::createType(const QString& text, const QIcon& icon, df::INode* prototype, int in, int out) const
{
    if (in > 0 && out > 0) {
        return createProcessing(text, icon, prototype, in, out);
    } else if (out > 0) {
        return createSource(text, icon, prototype, out);
    } else if (in > 0) {
        return createSink(text, icon, prototype, in);
    }

    UTILS_ASSERT(false);
    return boost::make_tuple((IVisualNode*)nullptr, Pins(), Pins());
}

SceneBuilder::SceneBuilder( CanvasStyleEditorPtr factories ) : 
     factories(factories)
{

}

IVisualConnectionPtr SceneBuilder::createConnection(IVisualOutputPinPtr outputPin, IVisualInputPinPtr inputPin)
{
    IVisualConnectionPtr connection = factories->getCurrentConnectionsFactory()->createConnection();
    connection->setInputPin(inputPin);
    connection->setOutputPin(outputPin);
	connection->visualItem()->setZValue(Z<IVisualItem::Connection, false>::value());
	connection->visualItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
    return connection;
}

SceneBuilder::VisualNodeWithPins vdf::SceneBuilder::createMerged( const QString& name, std::vector<IVisualInputPinPtr> ipins, std::vector<IVisualOutputPinPtr> opins )
{
	SceneBuilder::VisualNodeWithPins node = createType(name, QIcon(), nullptr, static_cast<int>(ipins.size()), static_cast<int>(opins.size()));
	return node;
}
