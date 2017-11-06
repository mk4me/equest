#include "TimeChannelWidget.h"
#include <QtGui/QPainter>

TimeChannelWidget::TimeChannelWidget(QWidget* parent, Qt::WindowFlags f, double begin, double end, double current) : QFrame(parent, f), begin(begin), end(end), current(current), currentTimeHalfWidth(0.01)
{
    UTILS_ASSERT((end <= 1.0 && begin <= end && begin >= 0.0), "Bledne krance kanalu w wizualizacji");
    UTILS_ASSERT((current >= 0.0 && current <= 1.0), "Bledny czas kanalu w wizualizacji");
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    
    setLineWidth ( 1 );
    setMidLineWidth ( 1 );
    setFrameShape ( QFrame::Box );
}

double TimeChannelWidget::getBegin() const
{ 
    return begin;
}

void TimeChannelWidget::setBegin(double begin) 
{ 
    UTILS_ASSERT((0.0 <= begin && begin <= end), "Bledny poczatek kanalu w wizualizacji");
    
    if(this->begin == begin){
        return;
    }

    this->begin = begin;
    update();
}

double TimeChannelWidget::getEnd() const
{ 
    return end;
}

void TimeChannelWidget::setEnd(double end) 
{ 
    UTILS_ASSERT((end <= 1.0 && begin <= end), "Bledny koniec kanalu w wizualizacji");
    if(this->end == end){
        return;
    }

    this->end = end;
    update();
}

double TimeChannelWidget::getCurrent() const
{
    return current;
}

void TimeChannelWidget::setCurrent(double current)
{
    UTILS_ASSERT((current <= 1.0 && 0.0 <= current), "Bledny czas kanalu");
    if(this->current == current){
        return;
    }

    this->current = current;
    update();
}

void TimeChannelWidget::setRange(double begin, double end)
{
    UTILS_ASSERT((end <= 1.0 && begin <= end && begin >= 0.0), "Bledne krance kanalu w wizualizacji");

    bool different = false;

    if(this->begin != begin){
        this->begin = begin;
        different = true;
    }

    if(this->end != end){
        this->end = end;
        different = true;
    }

    if(different == true){
        update();
    }
}

void TimeChannelWidget::paintEvent(QPaintEvent * pEvent)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing);
    QPen pen(Qt::black,0); // zero width pen is cosmetic pen
    //pen.setCosmetic(true);
    painter.setPen(pen);
    // We want to work with floating point, so we are considering
    // the rect as QRectF
    QRectF rect = this->rect();
    // at first fill the rect
    //painter.fillRect(rulerRect,QColor(220,200,180));
    painter.fillRect(rect,QColor(236,233,216));

    QRectF channelRect = rect;

    channelRect.setLeft(this->size().width() * begin);
    channelRect.setRight(this->size().width() * end);
    painter.fillRect(channelRect,QColor(0,0,0));

    QRectF channelCurrentTime = rect;
    channelCurrentTime.setLeft(std::max((current-currentTimeHalfWidth) * this->size().width(), 0.0));
    channelCurrentTime.setRight(std::min((current+currentTimeHalfWidth) * this->size().width(), (qreal)this->size().width()));

    painter.fillRect(channelCurrentTime,QColor(255,255,255,255));

    QFrame::paintEvent(pEvent);
}

void TimeChannelWidget::beginUpdate(double begin)
{
    setBegin(begin);
}

void TimeChannelWidget::endUpdate(double end)
{
    setEnd(end);
}
