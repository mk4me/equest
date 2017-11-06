#ifndef HEADER_GUARD__TIMELINETYPES_H__
#define HEADER_GUARD__TIMELINETYPES_H__

#include <vector>
#include <map>
#include <utils/SmartPtr.h>

/* Nagłówek z deklaracjami typow - deklaracje wyprzedzajace dla klas
* oraz odpowiednie typy kolekcji i wskaźnikow dla nich
*/

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
	////////////////////////////////////////////////////////////////////////////////

	class IChannel;
	typedef utils::shared_ptr<IChannel> IChannelPtr;
	typedef utils::weak_ptr<IChannel> IChannelWPtr;
	typedef std::vector<IChannelPtr> IChannels;

	typedef utils::shared_ptr<const IChannel> IChannelConstPtr;
	typedef utils::weak_ptr<const IChannel> IChannelConstWPtr;
	typedef std::vector<IChannelConstPtr> ConstIChannels;

	class Channel;
	typedef utils::shared_ptr<Channel> ChannelPtr;
	typedef utils::weak_ptr<Channel> ChannelWPtr;
	typedef std::vector<ChannelPtr> Channels;

	typedef utils::shared_ptr<const Channel> ChannelConstPtr;
	typedef utils::weak_ptr<const Channel> ChannelConstWPtr;
	typedef std::vector<ChannelConstPtr> ConstChannels;

	class Tag;
	typedef utils::shared_ptr<Tag> TagPtr;
	typedef utils::weak_ptr<Tag> TagWPtr;
	typedef std::vector<TagPtr> Tags;

	typedef utils::shared_ptr<const Tag> TagConstPtr;
	typedef utils::weak_ptr<const Tag> TagConstWPtr;
	typedef std::vector<TagConstPtr> ConstTags;

	class Model;
	typedef utils::shared_ptr<Model> ModelPtr;
	typedef utils::weak_ptr<Model> ModelWPtr;

	typedef utils::shared_ptr<const Model> ModelConstPtr;
	typedef utils::weak_ptr<const Model> ModelConstWPtr;

	class View;
	typedef utils::shared_ptr<View> ViewPtr;
	typedef utils::weak_ptr<View> ViewWPtr;

	typedef utils::shared_ptr<const View> ViewConstPtr;
	typedef utils::weak_ptr<const View> ViewConstWPtr;

	class IController;
	typedef utils::shared_ptr<IController> ControllerPtr;
	typedef utils::shared_ptr<const IController> ControllerConstPtr;
	typedef utils::weak_ptr<IController> ControllerWPtr;
	typedef utils::weak_ptr<const IController> ControllerConstWPtr;

	////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETYPES_H__
