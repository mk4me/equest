/********************************************************************
    created:  2011/08/08
    created:  8:8:2011   8:05
    filename: ITimelineService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__ITIMELINESERVICE_H__
#define HEADER_GUARD_TIMELINE__ITIMELINESERVICE_H__

#include <corelib/IService.h>
#include <timelinelib/IChannel.h>
#include <timelinelib/Controller.h>
#include <functional>

class TimelineWidget;

class ITimelineService
{
public:

    typedef std::function<void (const timeline::IChannelConstPtr &)> UIChannelAction;


public:
    virtual ~ITimelineService() {};


public:
    //! \param stream Strumień do dodania.
    virtual void addChannel(const std::string & path, const timeline::IChannelPtr & channel) = 0;

    virtual void removeChannel(const std::string & path) = 0;
    //! \return true jeżeli timeline sam się odtwarza.
    virtual bool isPlaying() const = 0;
    //!
    virtual void setPlaying(bool playing) = 0;
    //!
    virtual double getTime() const = 0;
    //!
    virtual void setTime(double time) = 0;
    //!
    virtual double getLength() const = 0;
    virtual double getNormalizedTime() const = 0;
    virtual void setNormalizedTime(double time) = 0;

    //!
    virtual void setChannelTooltip(const std::string & path, const std::string & tooltip) = 0;
    //!
    virtual std::string getChannelTooltip(const std::string & path) const = 0;
    //!
    virtual void setOnChannelClick(const UIChannelAction & action) = 0;
    //!
    virtual const UIChannelAction & getOnChannelClick() const = 0;
    //!
    virtual void setOnChannelDblClick(const UIChannelAction & action) = 0;
    //!
    virtual const UIChannelAction & getOnChannelDblClick() const = 0;

    virtual void setOnChannelRemove(const UIChannelAction & action) = 0;

    virtual const UIChannelAction & getOnChannelRemove() const = 0;

public:

    //! \return Kontroler.
    virtual timeline::IController* getController() = 0;
    //! \return Kontroler.
    virtual const timeline::IController* getController() const = 0;

};

typedef utils::shared_ptr<ITimelineService> TimelinePtr;


#endif  // __HEADER_GUARD__ITIMELINESERVICE_H__
