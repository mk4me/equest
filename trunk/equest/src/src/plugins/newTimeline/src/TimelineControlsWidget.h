/********************************************************************
    created:  2011/08/19
    created:  19:8:2011   9:46
    filename: TimelineControlsWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMELINECONTROLSWIDGET_H__
#define HEADER_GUARD_TIMELINE__TIMELINECONTROLSWIDGET_H__

#include "ui_TimelineControlsWidget.h"
#include <QtWidgets/QWidget>

class TimelineControlsWidget : public QWidget, public Ui::TimelineControlsWidget
{
    Q_OBJECT;

public:
    TimelineControlsWidget(QWidget * parent = nullptr);
    ~TimelineControlsWidget();

};

#endif  //  HEADER_GUARD_TIMELINE__TIMELINECONTROLSWIDGET_H__
