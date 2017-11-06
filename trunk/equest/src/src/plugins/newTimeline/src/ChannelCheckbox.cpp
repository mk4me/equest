#include "TimelinePCH.h"
#include "ChannelCheckbox.h"

ChannelCheckBox::ChannelCheckBox(const timeline::Model::TChannelConstPtr & channel) : channel(channel)
{

}

const timeline::Model::TChannelConstPtr & ChannelCheckBox::getChannel() const
{
    return channel;
}
