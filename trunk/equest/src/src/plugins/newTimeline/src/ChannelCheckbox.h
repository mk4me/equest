/********************************************************************
    created:  2011/08/05
    created:  5:8:2011   17:18
    filename: ChanelCheckbox.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANELCHECKBOX_H__
#define HEADER_GUARD_TIMELINE__CHANELCHECKBOX_H__

#include <QtWidgets/QCheckBox>
#include <timelinelib/Model.h>

class ChannelCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    ChannelCheckBox(const timeline::Model::TChannelConstPtr & channel);

    const timeline::Model::TChannelConstPtr & getChannel() const;

private:
    timeline::Model::TChannelConstPtr channel;
};

#endif  //  HEADER_GUARD_TIMELINE__CHANELCHECKBOX_H__
