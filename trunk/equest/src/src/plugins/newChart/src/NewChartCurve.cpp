#include "NewChartPCH.h"
#include "NewChartSeriesData.h"
#include "NewChartCurve.h"

NewChartCurve::NewChartCurve( const QString &title /*= QString::null */ ) :
    QwtPlotCurve(title),
    xOffset(0.0),
    yOffset(0.0),
    xScale(1.0),
    yScale(1.0)
{
}

void NewChartCurve::setSamples( NewChartSeriesData* data )
{
    setData(data);
}

void NewChartCurve::setXOffset( double val )
{
    setOffset(QPointF(val, yOffset));
}

void NewChartCurve::setYOffset( double val )
{
    setOffset(QPointF(xOffset, val));
}

void NewChartCurve::setOffset(const QPointF& point)
{
    xOffset = point.x();
    yOffset = point.y();
    NewChartSeriesData* samples = dynamic_cast<NewChartSeriesData*>(data());
    UTILS_ASSERT(samples);
    if (samples) {
        samples->setOffset(point);
    }
}

QPointF NewChartCurve::getOffset() const
{
    return QPointF(getXOffset(), getYOffset());
}

void NewChartCurve::setXScale( double val )
{
     xScale = val;
     NewChartSeriesData* samples = dynamic_cast<NewChartSeriesData*>(data());
     UTILS_ASSERT(samples);
     if (samples) {
         samples->setScaleX(xScale);
     }
}

void NewChartCurve::setYScale( double val )
{
    yScale = val; 
    NewChartSeriesData* samples = dynamic_cast<NewChartSeriesData*>(data());
    UTILS_ASSERT(samples);
    if (samples) {
        samples->setScaleY(yScale);
    }
}


