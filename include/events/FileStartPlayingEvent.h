#ifndef FILE_START_PLAYING_EVENT_H__
#define FILE_START_PLAYING_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class FileStartPlayingEvent : public Event
{
public:
    FileStartPlayingEvent(Object & sender) :
    Event(sender){ }

    virtual ~FileStartPlayingEvent() { }
};

#endif
