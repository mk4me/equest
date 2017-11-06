/********************************************************************
    created:  2011/04/06
    created:  6:4:2011   10:10
    filename: ChannelsTreeHeaderWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANNELSTREEHEADERWIDGET_H__
#define HEADER_GUARD_TIMELINE__CHANNELSTREEHEADERWIDGET_H__

#include <QtWidgets/QWidget>

class ChannelsTreeHeaderWidget : public QWidget
{
    Q_OBJECT;
public:
    ChannelsTreeHeaderWidget(QWidget * parent = 0);
    ~ChannelsTreeHeaderWidget();

    public slots:

        void setStartTime(double startTime);

        void setLength(double length);

        void setMainStep(double mainStep);

protected:

    virtual void paintEvent(QPaintEvent * pEvent);

public:
    double getStartTime() const;
    double getLength() const;
    double getMainStep() const;

private:

    double startTime;

    double length;

    double mainStep;
};

#endif  //HEADER_GUARD_TIMELINE__CHANNELSTREEHEADERWIDGET_H__
