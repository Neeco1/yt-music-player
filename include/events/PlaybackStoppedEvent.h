#ifndef PLAYBACK_STOPPED_EVENT_H__
#define PLAYBACK_STOPPED_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class PlaybackStoppedEvent : public Event
{
public:
    PlaybackStoppedEvent(Object & sender, std::string const & msg) :
    Event(sender),
    msg(msg) {
    }

    virtual ~PlaybackStoppedEvent() { }

    std::string const & getMessage() {
        return msg;
    }

private:
    std::string const & msg;
    
};

#endif
