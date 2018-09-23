#ifndef YOUTUBE_PLAYLIST_H__
#define YOUTUBE_PLAYLIST_H__

class Playlist;

#include "Playlist.h"
#include <thread>
#include <string>
#include "jsoncpp/json/json.h"

class YoutubePlaylist : public Playlist {
private:
    std::string playlistUrl;
    const std::string mpvSocketUrl = "/tmp/mpvsocket";
    
public:
    YoutubePlaylist(std::string playlistUrl);
    YoutubePlaylist(Json::Value json);
    
    void setPlaylistUrl(std::string playlistUrl);
    
    void playTrack(int trackIndex);
    void playList();
    void stopPlayback();
    void pausePlayback();
    
    Json::Value getJson();
};

#endif
