/********************************************************************
    created:  2011/07/18
    created:  18:7:2011   13:56
    filename: ChannelsTreeItem.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__
#define HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__

#include "TimelineService.h"
#include <QtWidgets/QTreeWidgetItem>
#include <timelinelib/Model.h>
#include "ChannelWidget.h"

//! Klasa reprezentująca kanał w drzewie UI
class ChannelsTreeItem : public QTreeWidgetItem
{
private:
    //! Kanał skojarzony z tym wpisem drzewa
    timeline::Model::TChannelConstPtr channelPtr;
    //! Widget z wizualna reprezentacją czasu kanału względem roota
    ChannelWidget * channelWidget;

public:
    ChannelsTreeItem(const timeline::Model::TChannelConstPtr & channelPtr, ChannelWidget * channelWidget);
    ~ChannelsTreeItem();

    //! \return Kanał timeline skojarzony z tym wpisem
    const timeline::Model::TChannelConstPtr & getChannel() const;

    void refreschVisualChannel();
};

#endif  //  HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__
