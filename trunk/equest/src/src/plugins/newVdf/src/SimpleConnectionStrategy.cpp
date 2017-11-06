#include "NewVdfPCH.h"
#include "SimpleConnectionStrategy.h"
#include <plugins/newVdf/IVisualPin.h>
#include "SceneModel.h"

using namespace vdf;

SimpleConnectionStrategy::SimpleConnectionStrategy()
{
}

void SimpleConnectionStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
{
    auto c = connection.lock();
	if (c && hasPins()) {
        path = QPainterPath();
	    
        QPointF start = beginPin.lock()->getConnectionPosHint(); 
	    QPointF finish = endPin.lock()->getConnectionPosHint();

	    path.moveTo(QPointF(start.x(), start.y()));
		path.moveTo(QPointF(finish.x(), finish.y()));
	    
        painter->setPen(QPen(c->isSelected() ? QColor(255, 50, 30) : QColor(), 2));
	    painter->drawPath(path);
    }
}

const QRectF& SimpleConnectionStrategy::getRect()
{
    if (hasPins()) {
	    QPointF start = beginPin.lock()->visualItem()->scenePos(); 
	    QPointF finish = endPin.lock()->visualItem()->scenePos();
	    int margin = 100;
	    if (start.y() < finish.y()) {
		    rect.setTop(start.y() - margin);
		    rect.setBottom(finish.y() + margin);
	    } else {
		    rect.setTop(finish.y() - margin);
		    rect.setBottom(start.y() + margin);
	    }
	    if (start.x() < finish.x()) {
		    rect.setLeft(start.x() - margin);
		    rect.setRight(finish.x() + margin);
	    } else {
		    rect.setLeft(finish.x() - margin);
		    rect.setRight(start.x() + margin);
	    }
    }
	return rect;
}

QPainterPath vdf::SimpleConnectionStrategy::shape() const
{
    return path;
}

void vdf::SimpleConnectionStrategy::setConnection( utils::weak_ptr<IVisualConnection> connection )
{
    this->connection = connection;
}

bool vdf::SimpleConnectionStrategy::hasPins()
{
    auto p1 = beginPin.lock();
    auto p2 = endPin.lock();
    if (!p2 || !p1) {
        auto c = connection.lock();
        beginPin = c->getInputPin();
        endPin = c->getOutputPin();
        p1 = beginPin.lock();
        p2 = beginPin.lock();
    }

    return p1 && p2;
}
