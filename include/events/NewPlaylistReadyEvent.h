#ifndef NEW_PLAYLIST_READY_EVENT_H__
#define NEW_PLAYLIST_READY_EVENT_H__

#include "EventBus/Object.hpp"
#include "EventBus/Event.hpp"

class NewPlaylistReadyEvent : public Event
{
public:
    NewPlaylistReadyEvent(Object & sender, std::shared_ptr<Playlist> playlist) :
    Event(sender),
    playlist(playlist) {
    }

    virtual ~NewPlaylistReadyEvent() { }

    std::shared_ptr<Playlist> const & getPlaylist() {
        return playlist;
    }

private:
    std::shared_ptr<Playlist> playlist;
    
};

#endif
