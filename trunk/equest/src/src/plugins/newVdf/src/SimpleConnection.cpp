#include "NewVdfPCH.h"
#include "SimpleConnection.h"
#include "SimpleItem.h"

using namespace vdf;

SimpleConnection::SimpleConnection() :
    item(new QGraphicsLineItem())
{
}

void SimpleConnection::setInputPin( const IVisualInputPinPtr pin )
{
    UTILS_ASSERT(!inputPin);
    inputPin = pin;
    QGraphicsObject* object = dynamic_cast<QGraphicsObject*>(pin->visualItem());
    UTILS_ASSERT(object);
    QObject::connect(object, SIGNAL(transformChanged()), this, SLOT(update()));
    update();
}

void SimpleConnection::setOutputPin( const IVisualOutputPinPtr pin )
{
    UTILS_ASSERT(!outputPin);
    outputPin = pin;
    connect(dynamic_cast<QGraphicsObject*>(pin->visualItem()), SIGNAL(transformChanged()), this, SLOT(update()));
    update();
}

QGraphicsItem * SimpleConnection::visualItem() const
{
    return item;
}

void SimpleConnection::update()
{
    if (inputPin && outputPin) {
        auto p1 = inputPin->visualItem()->scenePos();
        auto p2 = outputPin->visualItem()->scenePos();
        item->setLine(p1.x(), p1.y(), p2.x(), p2.y());
        item->update();
    }
}

void SimpleConnection::setVisualStrategy( IVisualStrategyPtr strategy )
{
	UTILS_ASSERT(false);
}

IVisualInputPinPtr SimpleConnection::getInputPin()
{
	return inputPin;
}

IVisualOutputPinPtr SimpleConnection::getOutputPin()
{
	return outputPin;
}


bool vdf::SimpleConnection::isSelected() const
{
    return item->isSelected();
}

void vdf::SimpleConnection::setSelected( bool val )
{
    item->setSelected(val);
}
