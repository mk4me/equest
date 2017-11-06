#include "DarkPinStrategy.h"
#include <QtGui/QPainter>
#include <utils/Debug.h>

using namespace vdf;

DarkPinStrategy::DarkPinStrategy()
{
	setState(IPinStrategy::Normal);
}

void DarkPinStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(QColor(128, 213, 220)));
	painter->setBrush(QBrush(QColor(128, 213, 220)));

	QRectF r = getRect();
	painter->drawEllipse(r);

	QRadialGradient grad(6.0f,6.0f,8.0f);
	grad.setColorAt(0.0f, Qt::white);
	grad.setColorAt(0.5f, stateColor);
	painter->setBrush(grad);
    QPen p(QColor(64, 64, 64));
    p.setWidth(2);
	painter->setPen(p);

	float width = r.width() * 0.8f;
	float height = r.height() * 0.8f;
	float x = (r.width() - width) * 0.5f + 1;
	float y = (r.height() - height) * 0.5f + 1;
	painter->drawEllipse(x, y, width, height);
}

void DarkPinStrategy::setPin( IVisualPinWeakPtr item )
{
	UTILS_ASSERT(item.lock());
	pin = item;
}

const QRectF& DarkPinStrategy::getRect()
{
	rect.setRect(0, 0, 16, 16);
	return rect;
}

void DarkPinStrategy::setState( State s )
{
	switch(s) {
	case IPinStrategy::Connected:
		stateColor = QColor(128, 213, 220);
		break;

	case IPinStrategy::Connective:
		stateColor = QColor(220, 220, 220);
		break;

	case IPinStrategy::Normal:
		stateColor = QColor(25, 170, 220);
		break;

	case IPinStrategy::Unconnective:
		stateColor = QColor(250, 170, 22);
		break;

	default:
		UTILS_ASSERT(false);
	}
}


QPointF vdf::DarkPinStrategy::getConnectionEndShift()
{
    auto pinptr = pin.lock();
    if (pinptr) {
        QPointF p;
        p.setY(rect.height() / 2);
        if (pinptr->isType(IVisualItem::InputPin)) {
            p.setX(0.0f);
        } else {
            p.setX(rect.width());
        }

        return p;
    }

    return QPointF();
}
