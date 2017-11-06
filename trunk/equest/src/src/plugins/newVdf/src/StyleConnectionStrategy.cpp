#include "NewVdfPCH.h"
#include "StyleConnection.h"
#include "StyleItem.h"

StyleConnection::StyleConnection() :
    begin(nullptr),
    end(nullptr),
    item(new StyleItem())
{

}

void StyleConnection::setBegin( const IVisualPin* pin )
{
    UTILS_ASSERT(!begin);
    begin = pin;
    QGraphicsObject* object = dynamic_cast<QGraphicsObject*>(pin->visualItem());
    UTILS_ASSERT(object);
    QObject::connect(object, SIGNAL(transformChanged()), this, SLOT(update()));
    update();
}

void StyleConnection::setEnd( const IVisualPin* pin )
{
    UTILS_ASSERT(!end);
    end = pin;
    QGraphicsObject* object = dynamic_cast<QGraphicsObject*>(pin->visualItem());
    UTILS_ASSERT(object);
    connect(object, SIGNAL(transformChanged()), this, SLOT(update()));
	if (begin && end) {
		IConnectionStrategy* pin = dynamic_cast<IConnectionStrategy*>(item->getStrategy());
		pin->setPins(begin, end);
	}
    update();
}

QGraphicsItem * StyleConnection::visualItem() const
{
    return item;
}

void StyleConnection::update()
{
    if (begin && end) {
       //auto p1 = begin->visualItem()->scenePos();
       //auto p2 = end->visualItem()->scenePos();
       //item->setLine(p1.x(), p1.y(), p2.x(), p2.y());
        item->update();
    }
}

void StyleConnection::setVisualStrategy( IVisualStrategy* strategy )
{
	item->setStrategy(strategy);
}

