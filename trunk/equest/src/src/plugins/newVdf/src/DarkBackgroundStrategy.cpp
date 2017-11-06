#include "NewVdfPCH.h"
#include "DarkBackgroundStrategy.h"
#include <QtGui/QPainter>

using namespace vdf;

void DarkBackgroundStrategy::paint( QPainter* painter, const QRectF& rect)
{
	int step = 15;
	painter->fillRect(rect, QColor(77,77,77));
	painter->setPen(QColor(101,101,101));
	int y = rect.y();
	int x = rect.x();
	float y1 = y - (y % step);
	float x2 = rect.width() + rect.x();
	float y2 = rect.height() + rect.y();
	while (y1 <= y2) {
		painter->drawLine(x, y1, x2, y1);
		y1 += step;
	}
	float x1 = x - (x % step);
	while (x1 <= x2) {
		painter->drawLine(x1, y, x1, y2);
		x1 += step;
	}
}
