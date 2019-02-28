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

class PlaylistUpdater;

class MusicPlayer : public EventHandler<NewPlaylistReadyEvent>,
                    public EventHandler<FileEndPlayingEvent>,
                    public EventHandler<FileStartPlayingEvent>,
                    public EventHandler<PlaybackTimeUpdatedEvent>
{

typedef std::map<std::string, std::shared_ptr<Playlist>> PlaylistMap;

private:
    PlaylistMap playlists;
    std::shared_ptr<Playlist> currentPlaylist;
    std::shared_ptr<Playlist> defaultPlaylist;
    
    //Handler pointer for NewPlaylistReadyEvent
    std::vector<HandlerRegistration*> handlerRegs;
    
    //Three flags to prevent restart of playback after the press of a button
    //(event "FileEnd" will be fired after press and would retrigger a playback)
    bool stopPressed;
    bool prevPressed;
    bool nextPressed;
    //Same for playlist selection
    bool newPlaylistSelected;
    
    unsigned int currentPlaybackTime;
    
    PlaybackState playbackState;
    PlaybackMode playbackMode;
    
    PlaylistUpdater * playlistUpdater;
    
    bool stop();
    bool nextTrack(bool shuffled);
    bool previousTrack();

public:
    MusicPlayer();
    ~MusicPlayer();
    
    static std::string addPlaylistFromUrl(std::string url, std::string name);
    
    void readDataFromJsonFile();
    void writeDataToJsonFile();
    
    bool startPlayback();
    bool stopPlayback();
    bool pausePlayback();
    bool playNext(bool shuffled);
    bool playPrevious();
    bool playTrackFromCurrentListWithIndex(unsigned int index);
    bool setVolume(unsigned int vol);

    bool playMediaFromUrl(std::string & url);
    
    PlaybackInfo getPlaybackInfo() const;
    PlaybackState getPlaybackState() const;
    bool setPlaybackMode(PlaybackMode mode);
    
    bool addPlaylist(const std::shared_ptr<Playlist> playlist);
    std::shared_ptr<Playlist> getPlaylist(const std::string & playlist_id) const;
    
    bool updatePlaylists();
    
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
