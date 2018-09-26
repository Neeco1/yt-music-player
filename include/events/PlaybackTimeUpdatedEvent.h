#ifndef PLAYBACK_TIME_UPDATED_EVENT_H__
#define PLAYBACK_TIME_UPDATED_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class PlaybackTimeUpdatedEvent : public Event
{
private:
    unsigned int playbackTime;

public:
    PlaybackTimeUpdatedEvent(Object & sender, unsigned int time) :
    Event(sender), playbackTime(time) { }

    virtual ~PlaybackTimeUpdatedEvent() { }
    
    unsigned int getPlaybackTime() {
        return playbackTime;
    }
};

#endif
