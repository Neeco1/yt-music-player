#ifndef MUSICPLAYER_H__
#define MUSICPLAYER_H__

#include "Playlist.h"
#include "Track.h"
#include <memory>
#include "EventBus/EventHandler.hpp"
#include "events/NewPlaylistReadyEvent.h"
#include "EventBus/HandlerRegistration.hpp"

class MusicPlayer : public EventHandler<NewPlaylistReadyEvent> {
private:
    std::vector<std::shared_ptr<Playlist>> playlists;
    std::shared_ptr<Playlist> currentPlaylist;
    
    HandlerRegistration* regNewPlaylistReady;

public:
    MusicPlayer();
    ~MusicPlayer();
    
    bool startPlaying();
    bool stopPlaying();
    bool setVolume(int vol);
    
    bool addPlaylist(const std::shared_ptr<Playlist> playlist);
    bool addPlaylistFromUrl(std::string & url);
    const std::vector<std::shared_ptr<Playlist>> & getPlaylists() const;
    const int getPlaylistCount() const;
    
    virtual void onEvent(NewPlaylistReadyEvent & e) override;
    
};


#endif
