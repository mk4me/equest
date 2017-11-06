/********************************************************************
    created:  2011/08/17
    created:  17:8:2011   14:20
    filename: TimeSliderWidget.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMESLIDERWIDGET_H__
#define HEADER_GUARD_TIMELINE__TIMESLIDERWIDGET_H__

#include <qwt_slider.h>
#include <QtCore/QTime>


class TimeSliderWidget : public QwtSlider
{
    Q_OBJECT;

public:
    explicit TimeSliderWidget(QWidget *parent = nullptr,
        Qt::Orientation orientation = Qt::Horizontal,
        ScalePosition scalePos = NoScale );

    virtual ~TimeSliderWidget();

    double getLeftMargin() const;

    double getRightMargin() const;

    const QwtScaleDraw * scaleDraw() const;

signals:

    void marginsChanged(double leftMargin, double rightMargin) const;

protected:

    virtual void drawSlider(QPainter *painter, const QRect &sliderRect ) const;

private:

    mutable double leftMargin;

    mutable double rightMargin;
};

#endif  //  HEADER_GUARD_TIMELINE__TIMESLIDERWIDGET_H__
