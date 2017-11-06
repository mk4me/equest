#include "ChannelsTreeItem.h"

ChannelsTreeItem::ChannelsTreeItem(timeline::Model::TChannelConstPtr channelPtr) : QTreeWidgetItem(), channelPtr(channelPtr)
{
	headers << "Active" << "Channel" << "Length" << "Global Time Scale" << "Local Time Scale" << "Global Offset" << "Local Offset" << "Visual Channel";

    channelsColumnIndex = headers.size() - 1;

	setText(1, QString::fromUtf8(channelPtr->getName().c_str()));
	setText(2, QString::number(channelPtr->getData()->getLength()));
	setText(3, QString::number(channelPtr->getData()->getGlobalTimeScale()));
	setText(4, QString::number(channelPtr->getData()->getLocalTimeScale()));
	setText(5, QString::number(channelPtr->getData()->getGlobalOffset()));
	setText(6, QString::number(channelPtr->getData()->getLocalOffset()));
}

ChannelsTreeItem::~ChannelsTreeItem()
{

}

const QStringList& ChannelsTreeItem::getHeaders() const
{
	return headers;
};

const timeline::Model::TChannelConstPtr & ChannelsTreeItem::getChannel() const
{
	return channelPtr;
}

int ChannelsTreeItem::getChannelsColumnIndex() const
{
    return channelsColumnIndex;
}
