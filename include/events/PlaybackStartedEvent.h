#ifndef PLAYBACK_STARTED_EVENT_H__
#define PLAYBACK_STARTED_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class PlaybackStartedEvent : public Event
{
public:
    PlaybackStartedEvent(Object & sender) :
    Event(sender){ }

    virtual ~PlaybackStartedEvent() { }
};

#endif

