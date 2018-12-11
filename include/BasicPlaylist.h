#ifndef BASIC_PLAYLIST_H__
#define BASIC_PLAYLIST_H__

class Playlist;

#include "Playlist.h"
#include "jsoncpp/json/json.h"

class BasicPlaylist : public Playlist  {
public:
    BasicPlaylist();
    BasicPlaylist(Json::Value json);
    ~BasicPlaylist() {}
    
    Json::Value getJson();
    
};

#endif
