#ifndef PLAYLIST_H__
#define PLAYLIST_H__

class Playlist;

#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>
#include "Track.h"
#include "Types.h"
#include "jsoncpp/json/json.h"
#include "Utils.h"

class Playlist {

protected:
    std::string listId;
    std::string name;
    std::vector<std::shared_ptr<Track>> tracks;
    std::vector<std::shared_ptr<Track>> shuffledOrder;
    PlaybackMode playbackMode;
    int currentTrack;
    bool nowPlaying;
    bool nowPaused;

public:
    Playlist();
    
    //Virtual destructor for base class to avoid resource leaks
    virtual ~Playlist() {}
    
    void setListId(const std::string & listId);
    std::string getListId();
    
    void setName(const std::string & name);
    const std::string & getName();
    
    void addTrack(const std::shared_ptr<Track> track);
    
    void setPlaybackMode(PlaybackMode playbackMode);
    const std::vector<std::shared_ptr<Track>> & getAllTracks() const;
    const int getTrackCount() const;
    bool isPlaying() const;
    bool isPaused() const;
    
    int getCurrentTrackNumber();
    std::shared_ptr<Track> getCurrentTrack();
    
    bool startPlaying();
    
    //Virtual method. Subclasses define here how a track can be played and stopped
    virtual void playTrack(int trackIndex) = 0;
    virtual void playList() = 0;
    virtual void stopPlayback() = 0;
    virtual void pausePlayback() = 0;
    
    virtual Json::Value getJson() = 0;
    
};

#endif
