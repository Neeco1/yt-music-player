#ifndef YOUTUBE_PLAYLIST_H__
#define YOUTUBE_PLAYLIST_H__

class Playlist;

#include "Playlist.h"
#include <string>
#include <atomic>
#include "jsoncpp/json/json.h"

class YoutubePlaylist : public Playlist {
private:
    std::string playlistUrl;
    std::atomic<bool> stopPlaybackFlag;
    
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
