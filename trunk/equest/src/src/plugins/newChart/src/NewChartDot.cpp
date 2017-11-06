#include "NewChartPCH.h"
#include "NewChartDot.h"

NewChartDot::NewChartDot( int size /*= 2*/) :
size(size)
{
    pen.setColor(Qt::darkBlue);
    brush.setColor(Qt::blue);
    brush.setStyle(Qt::SolidPattern);
    setZ( 1000 );
}

void NewChartDot::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const
{
    painter->setPen(pen);
    //painter->setBrush(brush);
    QBrush b;
    b.setStyle(Qt::NoBrush);
    painter->setBrush(b);

    QPoint transformed = QwtScaleMap::transform(xMap, yMap, getPosition()).toPoint();
    painter->drawEllipse(transformed, size, size);

    painter->setPen(QPen(QColor(Qt::black)));
    painter->drawEllipse(transformed, 1, 1);
}
