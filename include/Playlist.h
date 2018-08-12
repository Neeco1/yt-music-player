#ifndef PLAYLIST_H__
#define PLAYLIST_H__

#include <vector>
#include "Track.h"

class Playlist {
using string = std::string;

private:
    string listId;
    string name;
    std::vector<Track> tracks;

public:
    Playlist();
    
    void setListId(const string & listId) {
        this->listId = listId;
    }
    
    void setName(const string & name) {
        this->name = name;
    }
    
    void addTrack(const Track & track);
    const std::vector<Track> & getAllTracks() const;
    
};

#endif
