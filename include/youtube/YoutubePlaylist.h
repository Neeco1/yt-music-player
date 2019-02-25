#ifndef YOUTUBE_PLAYLIST_H__
#define YOUTUBE_PLAYLIST_H__

class Playlist;

#include "Playlist.h"
#include <string>
#include <atomic>
#include "jsoncpp/json/json.h"

class YoutubePlaylist : public Playlist {
public:
    YoutubePlaylist(std::string playlistUrl);
    YoutubePlaylist(Json::Value json);
    
    Json::Value getJson();
};

#endif
