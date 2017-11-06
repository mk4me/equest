/********************************************************************
    created:  2011/04/06
    created:  6:4:2011   10:10
    filename: ChannelsTree.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANNELSTREE_H__
#define HEADER_GUARD_TIMELINE__CHANNELSTREE_H__

#include "ChannelsTreeItem.h"
#include <timelinelib/Model.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>
#include <QtWidgets/QTreeWidget>

class ChannelsTree : public QTreeWidget, public utils::Observer<timeline::Model>
{
    Q_OBJECT;

private:

    timeline::ModelPtr model;
    timeline::ModelConstPtr constModel;

public:
                
    ChannelsTree(QWidget* parent = 0, const timeline::ModelPtr & model = timeline::ModelPtr());
    ~ChannelsTree();

    virtual void update(const timeline::Model * model);

    void setModel(const timeline::ModelPtr & model);
    void setModel(const timeline::ModelConstPtr & model);

    const timeline::ModelPtr & getModel();
    const timeline::ModelConstPtr & getModel() const;

    private slots:

        void refreshTime();

private:

    void refreshChannelTime(QTreeWidget * tree, QTreeWidgetItem * item, double time);

    void createTreeFromModel();

    static ChannelsTreeItem * createTreeItemFormChannel(timeline::Model::TChannelConstPtr channel);

    void initTimeChannelWidgets(ChannelsTreeItem * item, double refStart, double refLength, double normCurrentTime);
};

#endif  //HEADER_GUARD_TIMELINE__CHANNELSTREE_H__
