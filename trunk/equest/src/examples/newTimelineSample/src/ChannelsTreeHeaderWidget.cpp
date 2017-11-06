#include "ChannelsTreeHeaderWidget.h"
#include <QtGui/QPainter>
#include <utils/Debug.h>

ChannelsTreeHeaderWidget::ChannelsTreeHeaderWidget(QWidget * parent) : QWidget(parent), startTime(0.0), length(0.0), mainStep(5)
{
    this->setAutoFillBackground(true);
}

ChannelsTreeHeaderWidget::~ChannelsTreeHeaderWidget()
{

}

void ChannelsTreeHeaderWidget::setStartTime(double startTime)
{
    this->startTime = startTime;
    update();
}

void ChannelsTreeHeaderWidget::setLength(double length)
{
    this->length = length;
    update();
}

void ChannelsTreeHeaderWidget::setMainStep(double mainStep)
{
    UTILS_ASSERT((mainStep != 0.0), "Nieprawidlowy krok");
    this->mainStep = mainStep;
    update();
}

void ChannelsTreeHeaderWidget::paintEvent(QPaintEvent * pEvent)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing);

    QPen pen(Qt::black,0);
    painter.setPen(pen);
    painter.fillRect(rect(),QColor(0,255,0));
}

double ChannelsTreeHeaderWidget::getStartTime() const
{
    return startTime;
}

double ChannelsTreeHeaderWidget::getLength() const
{
    return length;
}

double ChannelsTreeHeaderWidget::getMainStep() const
{
    return mainStep;
}
