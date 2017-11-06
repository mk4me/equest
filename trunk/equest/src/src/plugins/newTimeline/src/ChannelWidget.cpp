#include "TimelinePCH.h"
#include "ChannelWidget.h"
#include <timelinelib/Tag.h>
#include <timelinelib/Controller.h>

ChannelWidget::ChannelWidget(const timeline::ControllerConstPtr & controller, const timeline::Model::TChannelConstPtr & channel,
    QWidget* parent, Qt::WindowFlags f, double leftMargin, double rightMargin, double currentTimeHalfWidth)
    : QFrame(parent, f), controller(controller), channel(channel), leftMargin(leftMargin), rightMargin(rightMargin),
    currentTimeHalfWidth(currentTimeHalfWidth), fillColor("cornflowerblue"),
    backgroundColor("white"), markerColor("gray")
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    setLineWidth ( 1 );
    setMidLineWidth ( 1 );
    setFrameShape ( QFrame::Box );
}

void ChannelWidget::setFillColor(const QColor & fillColor)
{
    if(this->fillColor != fillColor){
        this->fillColor = fillColor;
        update();
    }
}

void ChannelWidget::setBackgorundColor(const QColor & backgroundColor)
{
    if(this->backgroundColor != backgroundColor){
        this->backgroundColor = backgroundColor;
        update();
    }
}

void ChannelWidget::setMarkerColor(const QColor & markerColor)
{
    if(this->markerColor != markerColor){
        this->markerColor = markerColor;
        update();
    }
}

void ChannelWidget::paintEvent(QPaintEvent * pEvent)
{
    //ramka widgeta!!
    //QFrame::paintEvent(pEvent);

    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing);
    QPen pen(Qt::black,0); // zero width pen is cosmetic pen
    //pen.setCosmetic(true);
    painter.setPen(pen);
    // We want to work with floating point, so we are considering
    // the rect as QRectF
    QRectF rect = this->rect();

    painter.fillRect(rect, QColor("white"));

    rect.setLeft(rect.left() + leftMargin);
    rect.setRight(rect.right() - rightMargin);
    // at first fill the rect
    
    //wypełniamy tło
    painter.fillRect(rect,backgroundColor);

    QRectF channelRect = rect;

    qreal top = channelRect.top() + 2;
    qreal bottom = channelRect.bottom() - 2;

    if(top < bottom){
        channelRect.setTop(top);
        channelRect.setBottom(bottom);
    }
    
    //wypełaniamy kanał
    channelRect.setLeft(rect.left() + rect.width() * controller->getNormalizedTime(channel->getData()->getGlobalOffset()));
    channelRect.setRight(rect.left() + rect.width() * controller->getNormalizedTime(channel->getData()->getGlobalOffset() + channel->getData()->getLength()));
    painter.fillRect(channelRect,fillColor);

    //malujemy tagi
    for(auto it = channel->getData()->beginTags(); it != channel->getData()->endTags(); ++it){
        drawSingleTag(painter, channelRect, controller->getNormalizedTime((*it)->getBeginTime()), controller->getNormalizedLength((*it)->getLength()));
    }

    //rysujemy marker aktualnego czasu
    QRectF channelCurrentTime = rect;

    channelCurrentTime.setLeft(rect.left() + rect.width() * controller->getNormalizedTime(channel->getData()->getTime()) - 1);
    channelCurrentTime.setRight(rect.left() + rect.width() * controller->getNormalizedTime(channel->getData()->getTime()) + 1);    

    painter.fillRect(channelCurrentTime,markerColor);
}

void ChannelWidget::setMargins(double margin)
{
    setMargins(margin, margin);
}

void ChannelWidget::setMargins(double leftMargin, double rightMargin)
{
    bool changed = false;

    if(this->leftMargin != leftMargin){
        this->leftMargin = leftMargin;
        changed = true;
    }

    if(this->rightMargin != rightMargin){
        this->rightMargin = rightMargin;
        changed = true;
    }

    if(changed == true){
        update();
    }
}

double ChannelWidget::getLeftMargin() const
{
    return leftMargin;
}

double ChannelWidget::getRightMargin() const
{
    return rightMargin;
}

void ChannelWidget::drawSingleTag(QPainter & painter, const QRectF & rect, double normBegin, double normLength)
{
    painter.fillRect(rect.left() + rect.width() * normBegin - 2, rect.bottom(), std::max(2, (int)(rect.left() + rect.width() * (normBegin + normLength))), rect.top() - rect.bottom(), QColor("pink"));
}
