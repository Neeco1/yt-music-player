#ifndef PLAYLIST_H__
#define PLAYLIST_H__

#include <vector>
#include "Track.h"
#include "Types.h"

class Playlist {
using string = std::string;

private:
    string listId;
    string name;
    std::vector<Track> tracks;
    bool nowPlaying;
    int currentTrack;
    PlaybackMode playbackMode;

public:
    Playlist() {}
    //Virtual destructor for base class to avoid resource leaks
    virtual ~Playlist() {}
    
    void setListId(const string & listId) {
        this->listId = listId;
    }
    
    void setName(const string & name) {
        this->name = name;
    }
    const string & getName() {
        return name;
    }
    
    void addTrack(const Track & track) {
        tracks.push_back(track);
    }
    
    void setPlaybackMode(PlaybackMode playbackMode) {
        this->playbackMode = playbackMode;
    }
    
    const std::vector<Track> & getAllTracks() const {
        return tracks;
    }
    
    const int getTrackCount() const {
        return tracks.size();
    }
    
    bool isPlaying() const {
        return this->nowPlaying;
    }
    
    int getCurrentTrack() {
        return this->currentTrack;
    }
    
    bool startPlaying() {
        playTrack(0);
    }
    
    //Virtual method. Subclasses define here how a track can be played and stopped
    virtual void playTrack(int trackIndex) = 0;
    virtual void stopPlayback() = 0;
    
};

#endif
