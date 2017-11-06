#include "TimelinePCH.h"
#include "TimeSliderWidget.h"
#include <utils/TimeUtils.h>

class TimeScaleDraw : public QwtScaleDraw
{
public:

    TimeScaleDraw() {}
    virtual ~TimeScaleDraw() {}

    virtual QwtText label( double value) const
    {
		static const QString labelFormat("mm:ss:zzz");
        return QwtText(utils::convertToQTime(value).toString(labelFormat));
    }
};

TimeSliderWidget::TimeSliderWidget(QWidget *parent, Qt::Orientation orientation,
    ScalePosition scalePos)
    //: QwtSlider(parent, orientation, scalePos, bgStyle), leftMargin(0), rightMargin(0)
	: QwtSlider(orientation, parent), leftMargin(0), rightMargin(0)
{
    setScaleDraw(new TimeScaleDraw());
}

TimeSliderWidget::~TimeSliderWidget()
{

}

const QwtScaleDraw * TimeSliderWidget::scaleDraw() const
{
    return QwtSlider::scaleDraw();
}

void TimeSliderWidget::drawSlider(QPainter *painter, const QRect &sliderRect ) const
{
    QwtSlider::drawSlider(painter, sliderRect);

    bool changed = false;

    if(leftMargin != sliderRect.left() + handleSize().width() / 2){
        leftMargin = sliderRect.left() + handleSize().width() / 2;
        changed = true;
    }

    if(rightMargin != this->rect().right() - sliderRect.right() + handleSize().width() / 2){
        rightMargin = this->rect().right() - sliderRect.right() + handleSize().width() / 2;
        changed = true;
    }

    if(changed == true){
        emit marginsChanged(leftMargin, rightMargin);
    }
}

double TimeSliderWidget::getLeftMargin() const
{
    return leftMargin;
}

double TimeSliderWidget::getRightMargin() const
{
    return rightMargin;
}
