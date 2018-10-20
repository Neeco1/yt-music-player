#ifndef PLAYLIST_FETCH_START_EVENT_H__
#define PLAYLIST_FETCH_START_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class PlaylistFetchStartEvent : public Event
{
public:
    PlaylistFetchStartEvent(Object & sender, std::string playlist_id,
    std::string name)
    : Event(sender), playlist_id(playlist_id), name(name)
    {
    }

    virtual ~PlaylistFetchStartEvent() { }

    std::string getPlaylistId() {
        return playlist_id;
    }
    
    std::string getPlaylistName() {
        return name;
    }

private:
    std::string playlist_id, name;
    
};

#endif
