#include "TimelinePCH.h"
#include "ChannelsTreeItem.h"

ChannelsTreeItem::ChannelsTreeItem(const timeline::Model::TChannelConstPtr & channelPtr, ChannelWidget * channelWidget)
    : channelPtr(channelPtr), channelWidget(channelWidget)
{
    UTILS_ASSERT((channelPtr != nullptr), "Błędny kanał modelu timeline");
    UTILS_ASSERT((channelWidget != nullptr), "Błędny widget wizualizujący kanał");
    setText(0, QString::fromUtf8(channelPtr->getName().c_str()));
}

ChannelsTreeItem::~ChannelsTreeItem()
{

}

const timeline::Model::TChannelConstPtr & ChannelsTreeItem::getChannel() const
{
    return channelPtr;
}

void ChannelsTreeItem::refreschVisualChannel()
{
    channelWidget->update();
}
