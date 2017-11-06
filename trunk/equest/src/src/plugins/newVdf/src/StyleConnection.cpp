#include "NewVdfPCH.h"
#include "StyleConnection.h"
#include <plugins/newVdf/IConnectionStrategy.h>
#include "StyleItem.h"

using namespace vdf;

StyleConnection::StyleConnection() :
    item(new StyleItem())
{
}

void StyleConnection::setInputPin( const IVisualInputPinPtr pin )
{
    UTILS_ASSERT(!inputPin);
    inputPin = pin;
    QGraphicsObject* object = dynamic_cast<QGraphicsObject*>(pin->visualItem());
    UTILS_ASSERT(object);
    QObject::connect(object, SIGNAL(transformChanged()), this, SLOT(update()));
    update();
}

void StyleConnection::setOutputPin( const IVisualOutputPinPtr pin )
{
    UTILS_ASSERT(!outputPin);
    outputPin = pin;
    QGraphicsObject* object = dynamic_cast<QGraphicsObject*>(pin->visualItem());
    UTILS_ASSERT(object);
    connect(object, SIGNAL(transformChanged()), this, SLOT(update()));
    update();
}

QGraphicsItem * StyleConnection::visualItem() const
{
    return item;
}

void StyleConnection::update()
{
    if (inputPin && outputPin) {
        item->update();
    }
}

void StyleConnection::setVisualStrategy( IVisualStrategyPtr strategy )
{
	item->setStrategy(strategy);
}

IVisualInputPinPtr StyleConnection::getInputPin()
{
	return inputPin;
}

IVisualOutputPinPtr StyleConnection::getOutputPin()
{
	return outputPin;
}


bool vdf::StyleConnection::isSelected() const
{
    return item->isSelected();
}

void vdf::StyleConnection::setSelected( bool val )
{
    item->setSelected(val);
    if (val) {
        item->setZValue(Z<IVisualItem::Connection, true>::value());
    } else {                                
        item->setZValue(Z<IVisualItem::Connection, false>::value());
    }
}
