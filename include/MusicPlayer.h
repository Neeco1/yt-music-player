#ifndef MUSICPLAYER_H__
#define MUSICPLAYER_H__

#include "Playlist.h"
#include "Track.h"
#include "Types.h"
#include <memory>
#include "EventBus/EventHandler.hpp"
#include "events/NewPlaylistReadyEvent.h"
#include "EventBus/HandlerRegistration.hpp"

class MusicPlayer : public EventHandler<NewPlaylistReadyEvent> {

typedef std::map<std::string, std::shared_ptr<Playlist>> PlaylistMap;

private:
    PlaylistMap playlists;
    std::shared_ptr<Playlist> currentPlaylist;
    
    //Handler pointer for NewPlaylistReadyEvent
    HandlerRegistration* regNewPlaylistReady;

public:
    MusicPlayer();
    ~MusicPlayer();
    
    void readDataFromJsonFile();
    void writeDataToJsonFile();
    
    bool startPlayback();
    bool stopPlayback();
    bool pausePlayback();
    bool setVolume(unsigned int vol);
    
    PlaybackInfo getPlaybackInfo();
    bool setPlaybackMode(PlaybackMode mode);
    
    bool addPlaylist(const std::shared_ptr<Playlist> playlist);
    std::string addPlaylistFromUrl(std::string url, std::string name);
    const std::vector<std::shared_ptr<Playlist>> getPlaylists() const;
    const int getPlaylistCount() const;
    bool selectPlaylist(const std::string & playlist_id);
    
    
    virtual void onEvent(NewPlaylistReadyEvent & e) override;
    
};


#endif
