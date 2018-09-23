#ifndef NEW_PLAYLIST_FETCH_FAILED_EVENT_H__
#define NEW_PLAYLIST_FETCH_FAILED_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class NewPlaylistFetchFailedEvent : public Event
{
public:
    NewPlaylistFetchFailedEvent(Object & sender, std::string playlist_id) :
    Event(sender),
    playlist_id(playlist_id) {
    }

    virtual ~NewPlaylistFetchFailedEvent() { }

    std::string getPlaylistId() {
        return playlist_id;
    }

private:
    std::string playlist_id;
    
};

#endif
