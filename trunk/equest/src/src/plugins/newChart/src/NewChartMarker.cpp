#include "NewChartPCH.h"
#include "NewChartMarker.h"
#include <utils/Clamping.h>
#include <QtGui/QPainter>
#include "NewChartSerie.h"

const int LABEL_WIDTH = 200;
const int LABEL_HEIGHT = 45;
const int LABEL_PDIST = 20;
const int LABEL_BOUND = 3;
const float MIN_LERP_DIST = 15.0f;
const float LERP_Y = 0.1f;
const float LERP_X = 0.33f;

NewChartMarker::NewChartMarker(c3dlib::ScalarChannelReaderInterfaceConstPtr reader) :
    reader(reader),
    positionSet(false),
    lerpX(LERP_X),
    lerpY(LERP_Y)
{
    setZ(1001);
}

NewChartMarker::NewChartMarker() :
positionSet(false),
    lerpX(LERP_X),
    lerpY(LERP_Y)
{

}


void NewChartMarker::drawLines( QPainter *painter, const QRectF &rect, const QPointF & point ) const
{
    QwtPlotMarker::drawLines(painter, rect, point);
}

float lerp(float from, float to, float ratio)
{
    return from * (1.0f - ratio) + to * ratio;
}


void NewChartMarker::drawLabel( QPainter *painter, const QRectF &rect, const QPointF & point ) const
{
    drawDot(painter, point);

    int px = point.x();
    int py = point.y();

    if (px < rect.left() || px > rect.right() || py > rect.bottom() || py < rect.top()) {
        resetMomentum();
        return;
    }

    float destX = px + LABEL_PDIST;
    float destY = py + LABEL_PDIST;

    if ((destX + LABEL_WIDTH) > rect.right()) {
        destX = px - LABEL_PDIST - LABEL_WIDTH;
    }

    if ((destY + LABEL_HEIGHT) > rect.bottom()) {
        destY = py - LABEL_PDIST - LABEL_HEIGHT;
    }

    if (!positionSet) {
        position.setX(px + LABEL_PDIST);
        position.setY(py + LABEL_PDIST);
        positionSet = true;
    }
    
    
    if (abs(position.x() - destX) > MIN_LERP_DIST) {
        position.setX(lerp(position.x(), destX, lerpX));
    }

    if (abs(position.y() - destY) > MIN_LERP_DIST) {
       position.setY(lerp(position.y(), destY, lerpY)); 
    }
    
    

    painter->setPen(QPen(QColor(135, 173, 255)));
    painter->setBrush(QBrush(QColor(255,255,255)));

    int lineX = utils::clamp(point.x(), position.x(), position.x() + LABEL_WIDTH);
    int lineY = utils::clamp(point.y(), position.y(), position.y() + LABEL_HEIGHT);

    painter->drawLine(point.x() + 3, point.y() + 3, lineX, lineY);
   
	//QString text = QObject::tr("Time: %1\nValue: %2").arg(xValue()).arg(yValue());
	QString text1 = QObject::tr("Time: %1").arg(xValue());
	QString text2 = QObject::tr("Value: %1").arg(yValue());
	auto text = text1 + "\n" + text2;
	QFontMetrics fm(painter->font());

	QRect textRect;
	textRect.setX(static_cast<int>(position.x()));
	textRect.setY(static_cast<int>(position.y()));
	textRect.setWidth(LABEL_WIDTH);
	textRect.setHeight(LABEL_HEIGHT);

	textRect = fm.boundingRect(textRect, Qt::AlignCenter, text);
	auto w = textRect.width() + 20;
	auto h = textRect.height() + 10;
	textRect.setX(std::max(lineX - w / 2, 10));
	textRect.setY(position.y());
	textRect.setWidth(w);
	textRect.setHeight(h);
    //QRect textRect;
    //textRect.setX(static_cast<int>(position.x()));
    //textRect.setY(static_cast<int>(position.y()));
    //textRect.setWidth(LABEL_WIDTH);
    //textRect.setHeight(LABEL_HEIGHT);
    
    QRect boxRect;
    boxRect.setX(textRect.x() - LABEL_BOUND);
    boxRect.setY(textRect.y() - LABEL_BOUND);
    boxRect.setWidth(textRect.width() + LABEL_BOUND * 2);
    boxRect.setHeight(textRect.height() + LABEL_BOUND * 2);
   
    painter->drawRoundedRect(boxRect, 6, 6);

    painter->setPen(QPen(QColor(100, 100, 100)));
    painter->drawText(textRect, Qt::AlignCenter, text);
}

void NewChartMarker::drawDot( QPainter * painter, const QPointF & point, int size )
{
    painter->setPen(QPen(QColor(Qt::black)));
    painter->drawEllipse(point, 1, 1);

    painter->setPen(QPen(QColor(135, 173, 255)));
    
    QBrush b;
    b.setStyle(Qt::NoBrush);
    //painter->setBrush(QBrush(QColor(65, 113, 195, 128)));
    painter->setBrush(b);
    painter->drawEllipse(point, size, size);
}
