#include "NewChartPCH.h"
#include "NewChartLabel.h"
#include "NewChartDotFixed.h"
#include <utils/Clamping.h>

// nieladna kopia z NewChartMarker
const int LABEL_WIDTH = 200;
const int LABEL_HEIGHT = 45;
const int LABEL_PDIST = 20;
const int LABEL_BOUND = 3;
const float MIN_LERP_DIST = 15.0f;
const float LERP_Y = 0.1f;
const float LERP_X = 0.33f;

NewChartLabel::NewChartLabel(const QString& text, const QPoint& shift, const QPoint& size, bool closeable) :
text(text),
    size(size),
    shift(shift),
    pen(QColor(135, 173, 255)),
    brush(QColor(Qt::white)),
	points(new NewChartVerticalsConnection(NewChartVerticalsConnection::Simple, this)),
	closeable(closeable)
{
    setZ( 1000 );
    brush.setStyle(Qt::SolidPattern);
}


void NewChartLabel::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const
{
	UTILS_ASSERT(points->getPoint1());
	QPoint transformed1 = QwtScaleMap::transform(xMap, yMap, points->getPoint1()->getPosition()).toPoint();

	auto textRect = getTextRect(painter, transformed1);
	QRect boxRect = getBoxRect(textRect);

    painter->setPen(QPen(Qt::black));
    painter->setBrush(QBrush(Qt::black));

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRoundedRect(boxRect, 6, 6);

	if (closeable) {
		drawX(painter, boxRect);
	}

    painter->setPen(QPen(QColor(100, 100, 100)));
    painter->drawText(textRect, Qt::AlignCenter, text);

	this->boundingR.setRect(boxRect.x(), boxRect.y(), boxRect.width(), boxRect.height());
}

bool NewChartLabel::isInsideLabel( const QPoint& transformedPoint, const QwtPlotCurve* curve ) const
{
    QPoint pointingTransformed = getPoint1Transformed(curve);
	/*QRect rect;
	rect.setTopLeft(pointingTransformed + shift);
	rect.setWidth(size.x());
	rect.setHeight(size.y());*/
 
    return boundingR.contains(transformedPoint);
}

QPoint NewChartLabel::getPoint1Transformed(const QwtPlotCurve* curve) const
{
	UTILS_ASSERT(plot() && points->getPoint1());
    const double x = plot()->transform(curve->xAxis(), points->getPoint1()->getPosition().x() );
    const double y = plot()->transform(curve->yAxis(), points->getPoint1()->getPosition().y() );
    return QPoint(static_cast<int>(x), static_cast<int>(y));
}

QPoint NewChartLabel::getPoint2Transformed(const QwtPlotCurve* curve) const
{
	UTILS_ASSERT(plot() && points->getPoint2());
    const double x = plot()->transform(curve->xAxis(), points->getPoint2()->getPosition().x() );
    const double y = plot()->transform(curve->yAxis(), points->getPoint2()->getPosition().y() );
    return QPoint(static_cast<int>(x), static_cast<int>(y));
}

void NewChartLabel::connectDot(NewChartDotConstPtr dot, NewChartVerticalsConnection::ConnectionStyle style /*= Simple*/)
{
    this->points->setPoint1(dot);
    this->points->setPoint2(NewChartDotConstPtr());
	this->points->setConnectionStyle(style);
}

void NewChartLabel::connectDots(NewChartDotConstPtr point1, NewChartDotConstPtr point2, NewChartVerticalsConnection::ConnectionStyle style)
{
    this->points->setPoint1(point1);
	this->points->setPoint2(point2);
	this->points->setConnectionStyle(style);
}

void NewChartLabel::connectDots(const QPointF& point1, const QPointF& point2, NewChartVerticalsConnection::ConnectionStyle style)
{
    NewChartDotPtr p1(new NewChartDotFixed(point1));
    NewChartDotPtr p2(new NewChartDotFixed(point2));
    connectDots(p1, p2, style);
}

QRect NewChartLabel::getBoxRect(QPainter * painter, QPoint &transformed1) const
{
	auto r = getTextRect(painter, transformed1);
	return getBoxRect(r);
}

QRect NewChartLabel::getBoxRect(const QRect& textRect) const
{
	QRect boxRect;
	boxRect.setX(textRect.x() - LABEL_BOUND);
	boxRect.setY(textRect.y() - LABEL_BOUND);
	boxRect.setWidth(textRect.width() + LABEL_BOUND * 2);
	boxRect.setHeight(textRect.height() + LABEL_BOUND * 2);
	return boxRect;
}

QRect NewChartLabel::getTextRect(QPainter * painter, const QPoint &transformed1) const
{
	QFontMetrics fm(painter->font());

	QRect textRect;
	textRect.setX(transformed1.x() + shift.x());
	textRect.setY(transformed1.y() + shift.y());
	textRect.setWidth(LABEL_WIDTH);
	textRect.setHeight(LABEL_HEIGHT);

	textRect = fm.boundingRect(textRect, Qt::AlignCenter, text);
	auto w = textRect.width() + 20;
	auto h = textRect.height() + 10;
	textRect.setX(std::max((transformed1.x() + shift.x() + LABEL_WIDTH / 2) - w / 2, 10));
	textRect.setY(transformed1.y() + shift.y());
	textRect.setWidth(w);
	textRect.setHeight(h);
	return textRect;
}

QRectF NewChartLabel::boundingRect() const
{
	return boundingR;
}

void NewChartLabel::setVisible(bool visible)
{
	QwtPlotItem::setVisible(visible);
	points->setVisible(visible);
}

void NewChartLabel::drawX(QPainter * painter, const QRect& r) const
{
	QVector<QPoint> xLines;
	xLines.push_back(QPoint(r.right() - 2, r.top() + 2));
	xLines.push_back(QPoint(r.right() - 8, r.top() + 8));
	xLines.push_back(QPoint(r.right() - 8, r.top() + 2));
	xLines.push_back(QPoint(r.right() - 2, r.top() + 8));
	painter->drawLines(xLines);
}

bool NewChartLabel::isInsideClose(const QPoint& transformedPoint) const
{
	if (!closeable) {
		return false;
	}

	QRectF xR;
	auto& r = boundingR;
	xR.setRect(r.right() - 10, r.top(), 10, 10);
	return xR.contains(transformedPoint);
}

void NewChartVerticalsConnection::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect) const
{
	if (!point1) {
		return;
	}
	QPoint transformed1 = QwtScaleMap::transform(xMap, yMap, point1->getPosition()).toPoint();
	auto boxRect = label->getBoxRect(painter, transformed1);
	if (!point2) {
		drawConnection(painter, boxRect, transformed1, connectionStyle);
	} else {
		QPoint transformed2 = QwtScaleMap::transform(xMap, yMap, point2->getPosition()).toPoint();

		if (connectionStyle == Horizontal) {
			if (transformed1.x() > transformed2.x()) {
				QPoint temp = transformed1;
				transformed1 = transformed2;
				transformed2 = temp;
			}

			drawConnection(painter, boxRect, transformed1, connectionStyle, boxRect.left() > transformed2.x());
			drawConnection(painter, boxRect, transformed2, connectionStyle, boxRect.right() < transformed1.x());
		} else if (connectionStyle == Vertical) {
			if (transformed1.y() > transformed2.y()) {
				QPoint temp = transformed1;
				transformed1 = transformed2;
				transformed2 = temp;
			}
			drawConnection(painter, boxRect, transformed1, connectionStyle, boxRect.top() > transformed2.y());
			drawConnection(painter, boxRect, transformed2, connectionStyle, boxRect.bottom() < transformed1.y());
		} else {
			UTILS_ASSERT(false);
		}
	}
}

NewChartVerticalsConnection::NewChartVerticalsConnection(ConnectionStyle connectionStyle, NewChartLabel* label) : 
	label(label),
	point1(nullptr),
	point2(nullptr),
	connectionStyle(NewChartVerticalsConnection::Simple)
{

}

void NewChartVerticalsConnection::drawConnection(QPainter* painter, const QRect& box, const QPoint& transformedTo, ConnectionStyle style, bool arrowOutside /*= false*/) const
{
	switch (style) {
	case Simple: {
		int pX = utils::clamp(transformedTo.x(), box.left(), box.right());
		int pY = utils::clamp(transformedTo.y(), box.top(), box.bottom());
		drawArrow(painter, QPoint(pX, pY), transformedTo, arrowOutside);
	} break;

	case Horizontal: {
		QPoint transformedFrom(box.left(), box.top() + box.height() / 2);
		QPoint p(transformedTo.x(), transformedFrom.y());
		drawArrow(painter, transformedFrom, p, arrowOutside);
		painter->drawLine(transformedTo, p);
	} break;

	case Vertical: {
		QPoint transformedFrom(box.left() + box.width() / 2, box.top());
		QPoint p(transformedFrom.x(), transformedTo.y());
		drawArrow(painter, transformedFrom, p, arrowOutside);
		painter->drawLine(p, transformedTo);
	} break;

	default:
		UTILS_ASSERT(false);
	}
}

void NewChartVerticalsConnection::drawArrow(QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo, bool outside /*= false*/) const
{
	painter->drawLine(transformedFrom, transformedTo);
	QPointF normal = (transformedTo - transformedFrom);
	float length = sqrt(normal.x() * normal.x() + normal.y() * normal.y());
	if (length < 0.0001f) {
		return;
	}
	normal.setX(normal.x() / length);
	normal.setY(normal.y() / length);

	QPointF normalR(-normal.y(), normal.x());
	QPointF base = outside ? (transformedTo + (7 * normal)) : (transformedTo - (7 * normal));
	QPointF pL = base + normalR * 2;
	QPointF pR = base - normalR * 2;

	QPolygon triangle;
	triangle.clear();
	triangle << pL.toPoint();
	triangle << transformedTo;
	triangle << pR.toPoint();

	painter->drawPolygon(triangle);
}
