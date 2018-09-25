#ifndef FILE_END_PLAYING_EVENT_H__
#define FILE_END_PLAYING_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class FileEndPlayingEvent : public Event
{
public:
    FileEndPlayingEvent(Object & sender) :
    Event(sender){ }

    virtual ~FileEndPlayingEvent() { }
};

#endif
