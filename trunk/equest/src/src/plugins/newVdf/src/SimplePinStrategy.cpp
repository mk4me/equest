#include "SimplePinStrategy.h"
#include <QtGui/QPainter>
#include <utils/Debug.h>

using namespace vdf;

SimplePinStrategy::SimplePinStrategy()
{
	setState(IPinStrategy::Normal);
}

void SimplePinStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
{
	painter->setBrush(QBrush(stateColor));
	QRectF r = getRect();
	painter->drawRect(r);
}

void SimplePinStrategy::setPin( IVisualPinWeakPtr item )
{
	UTILS_ASSERT(item.lock());
	pin = item;
}

const QRectF& SimplePinStrategy::getRect()
{
	rect.setRect(0, 0, 8, 8);
	return rect;
}

void SimplePinStrategy::setState( State s )
{
	switch(s) {
	case IPinStrategy::Connected:
		stateColor = QColor(200, 200, 0);
		break;

	case IPinStrategy::Connective:
		stateColor = QColor(220, 220, 0);
		break;

	case IPinStrategy::Normal:
		stateColor = QColor(120, 120, 120);
		break;

	case IPinStrategy::Unconnective:
		stateColor = QColor(250, 170, 22);
		break;

	default:
		UTILS_ASSERT(false);
	}
}


QPointF vdf::SimplePinStrategy::getConnectionEndShift()
{
    QPointF p;
    p.setY(rect.height() / 2);
    p.setX(rect.width() / 2);
	return p;
}
