#ifndef MUSICPLAYER_H__
#define MUSICPLAYER_H__

#include "Playlist.h"
#include "Track.h"
#include "Types.h"
#include <memory>
#include <vector>
#include "EventBus/EventHandler.hpp"
#include "EventBus/HandlerRegistration.hpp"
#include "events/NewPlaylistReadyEvent.h"
#include "events/FileEndPlayingEvent.h"
#include "events/FileStartPlayingEvent.h"
#include "events/PlaybackTimeUpdatedEvent.h"

class MusicPlayer : public EventHandler<NewPlaylistReadyEvent>,
                    public EventHandler<FileEndPlayingEvent>,
                    public EventHandler<FileStartPlayingEvent>,
                    public EventHandler<PlaybackTimeUpdatedEvent>
{

typedef std::map<std::string, std::shared_ptr<Playlist>> PlaylistMap;

private:
    PlaylistMap playlists;
    std::shared_ptr<Playlist> currentPlaylist;
    
    //Handler pointer for NewPlaylistReadyEvent
    std::vector<HandlerRegistration*> handlerRegs;
    
    bool stopPressed;
    bool prevPressed;
    bool nextPressed;
    
    unsigned int currentPlaybackTime;

public:
    MusicPlayer();
    ~MusicPlayer();
    
    void readDataFromJsonFile();
    void writeDataToJsonFile();
    
    bool startPlayback();
    bool stopPlayback();
    bool pausePlayback();
    bool nextTrack();
    bool previousTrack();
    bool setVolume(unsigned int vol);
    
    PlaybackInfo getPlaybackInfo();
    bool setPlaybackMode(PlaybackMode mode);
    
    bool addPlaylist(const std::shared_ptr<Playlist> playlist);
    std::string addPlaylistFromUrl(std::string url, std::string name);
    const std::vector<std::shared_ptr<Playlist>> getPlaylists() const;
    const int getPlaylistCount() const;
    bool selectPlaylist(const std::string & playlist_id);
    
    bool setPlaybackTime(std::string time);
    
    
    virtual void onEvent(NewPlaylistReadyEvent & e) override;
    virtual void onEvent(FileStartPlayingEvent & e) override;
    virtual void onEvent(FileEndPlayingEvent & e) override;
    virtual void onEvent(PlaybackTimeUpdatedEvent & e) override;
    
};


#endif
