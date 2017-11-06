#include "NewChartPCH.h"
#include "NewChartSeriesData.h"

NewChartSeriesData::NewChartSeriesData(c3dlib::ScalarChannelReaderInterfaceConstPtr channel,
    QPointF offset, double scaleX , double scaleY) :
    channel(channel),
    offset(offset),
    scaleX(scaleX),
    scaleY(scaleY)
{

}

NewChartSeriesData::~NewChartSeriesData()
{
}

size_t NewChartSeriesData::size() const
{
    return channel->size();
}

QPointF NewChartSeriesData::sample( size_t i ) const
{
	const auto sample = channel->sample(i);
    return QPointF(sample.first * scaleX, sample.second * scaleY) + offset;
}

QRectF NewChartSeriesData::boundingRect() const
{
    return QRectF(0, 1000, 20, 2000);
}
