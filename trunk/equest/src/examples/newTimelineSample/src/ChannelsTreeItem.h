/********************************************************************
    created:  2011/04/06
    created:  6:4:2011   10:10
    filename: ChannelsTreeItem.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__
#define HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__

#include <timelinelib/Model.h>
#include <QtWidgets/QTreeWidgetItem>

class ChannelsTreeItem : public QTreeWidgetItem
{

private:
    QStringList headers;
    timeline::Model::TChannelConstPtr channelPtr;

public:
    ChannelsTreeItem(timeline::Model::TChannelConstPtr channelPtr);
    ~ChannelsTreeItem();

    const QStringList& getHeaders() const;

    int getChannelsColumnIndex() const;

    const timeline::Model::TChannelConstPtr & getChannel() const;

private:
    int channelsColumnIndex;
};


#endif	//HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__
