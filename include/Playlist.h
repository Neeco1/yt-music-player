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
#include "EventBus/Object.hpp"
#include "EventBus/EventBus.hpp"

class Playlist : public Object  {

protected:
    std::string listId;
    std::string name;
    std::vector<std::shared_ptr<Track>> tracks;
    std::vector<std::shared_ptr<Track>> shuffledOrder;
    int currentTrack;
    
    std::string playlistUrl;
    
    bool playing;
    bool isShuffled;

public:
    Playlist();
    
    //Virtual destructor for base class to avoid resource leaks
    virtual ~Playlist() {}
    
    void setPlaylistUrl(std::string playlistUrl);
    std::string getPlaylistUrl();
    
    void setListId(const std::string & listId);
    std::string getListId();
    
    void setName(const std::string & name);
    const std::string & getName();
    
    void addTrack(const std::shared_ptr<Track> track);
    const std::vector<std::shared_ptr<Track>> & getAllTracks() const;
    const unsigned int getTrackCount() const;
    
    void setPlaying(bool value);
    bool isPlaying();
    
    void setCurrentTrackNumber(unsigned int track);
    unsigned int getCurrentTrackNumber();
    std::shared_ptr<Track> getCurrentTrack();
    
    std::shared_ptr<Track> nextTrack();
    std::shared_ptr<Track> previousTrack();
    
    void shuffleList();
    void clearShuffle();

    virtual Json::Value getJson() = 0;
    
};

#endif
