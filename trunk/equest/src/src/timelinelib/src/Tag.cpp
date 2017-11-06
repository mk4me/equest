#include <timelinelib/Tag.h>
#include <timelinelib/Channel.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

Tag::Tag(const ChannelPtr & channel, const std::string & name, double begin, double length)
    : name(name), channel(channel), constChannel(channel), begin(begin), length(length)
{
    UTILS_ASSERT((channel != nullptr), "Niepoprawny kanał taga");
    UTILS_ASSERT((begin >= 0 && length >= 0 && begin + length <= channel->getLength()), "Niepoprawna konfiguracja czasowa taga");
}

const std::string & Tag::getName() const
{
    return name;
}

const ChannelConstWPtr & Tag::getChannel() const
{
    return constChannel;
}

double Tag::getLength() const
{
    return length;
}

const ChannelWPtr & Tag::getChannel()
{
    return channel;
}

double Tag::getBeginTime() const
{
    return begin;
}

void Tag::setName(const std::string & name)
{
    if(this->name == name){
        return;
    }

    if(constChannel.lock() == nullptr || constChannel.lock()->findTag(name) == constChannel.lock()->endTags()){
        this->name = name;
    }else{
        throw std::invalid_argument("Tag with given name already exist in channel!");
    }
}

void Tag::setBeginTime(double begin)
{
    if(begin < 0 || begin + length > channel.lock()->getLength()) {
        throw std::invalid_argument("Wrong tag begin in channel!");
    }

    this->begin = begin;
}

void Tag::setLength(double length)
{
    if(length < 0 || begin + length > channel.lock()->getLength()) {
        throw std::invalid_argument("Wrong tag length in channel!");
    }

    this->length = length;
}

void Tag::shiftTag(double dTime)
{
    if(begin + dTime < 0 || begin + dTime + length > channel.lock()->getLength()) {
        throw std::invalid_argument("Wrong tag time shift value!");
    }

    this->begin += dTime;
}

void Tag::resetChannel()
{
    channel = ChannelPtr();
    constChannel = ChannelConstPtr();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////
