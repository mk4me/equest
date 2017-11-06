/********************************************************************
    created:  2011/04/06
    created:  6:4:2011   10:10
    filename: TimelineWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMELINEWIDGET_H__
#define HEADER_GUARD_TIMELINE__TIMELINEWIDGET_H__

#include "ui_TimelineWidget.h"
#include "ChannelsTreeHeaderWidget.h"
#include <timelinelib/Model.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>
#include <QtCore/QSharedPointer>
#include <QtCore/QWeakPointer>
#include "ChannelsTreeItem.h"
#include "TimeChannelWidget.h"
#include "TimeCounter.h"
#include <boost/shared_ptr.hpp>

class TimelineWidget : public QWidget, public Ui::TimelineWidget, public utils::Observer<timeline::Model>
{
    Q_OBJECT;

    private:

        timeline::ModelPtr model;
        timeline::ModelConstPtr constModel;
        TimeCounter timeCounter;

    public:
        TimelineWidget(const timeline::ModelPtr & model);
        ~TimelineWidget();

        void setModel(const timeline::ModelPtr & model);

        const timeline::ModelPtr & getModel();
        const timeline::ModelConstPtr & getModel() const;

        virtual void update(const timeline::Model * model);

protected:

        virtual void resizeEvent(QResizeEvent * qResizeEvent);

    protected slots:

        void toBegin();
        void toEnd();
        void play();
        void setTime(int time);

        void updateTimeCursor();

        void refreshTime();

    private:

        QWidget * getVisualChannelsColumnWidget() const;

        int getChannelsHeight(QTreeWidgetItem * item) const;

        int getCursorTop() const;

        int getCursorXMin() const;

        int getCursorXMax() const;

        void refreshChannelTime(QTreeWidget * tree, QTreeWidgetItem * item, double time);

        void createTreeFromModel();

        static ChannelsTreeItem * createTreeItemFormChannel(timeline::Model::TChannelConstPtr channel);

        void initTimeChannelWidgets(ChannelsTreeItem * item, double refStart, double refLength, double normCurrentTime);

private:

    bool isPlaying;

    QSharedPointer<QWidget> overlay;

    QSharedPointer<ChannelsTreeHeaderWidget> visualChannelsColumnHeader;
};

#endif  //HEADER_GUARD_TIMELINE__TIMELINEWIDGET_H__
