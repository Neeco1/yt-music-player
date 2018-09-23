#include "youtube/YoutubePlaylist.h"
#include <sstream>
#include <iostream>
#include "jsoncpp/json/json.h"
#include <stdlib.h>

YoutubePlaylist::YoutubePlaylist(std::string playlistUrl)
: Playlist(), playlistUrl(playlistUrl)
{ }

YoutubePlaylist::YoutubePlaylist(Json::Value json)
: Playlist()
{
    setListId(json["id"].asString());
    setName(json["name"].asString());
    setPlaylistUrl(json["url"].asString());
    for(Json::Value track : json["tracks"])
    {
        addTrack(std::make_shared<Track>(track));
    }
}

void YoutubePlaylist::setPlaylistUrl(std::string playlistUrl) {
    this->playlistUrl = playlistUrl;
}

void YoutubePlaylist::playTrack(int trackIndex) {
    if(isPlaying() || isPaused())
    {
        stopPlayback();
    }
    
    auto track = tracks[trackIndex];
    
    std::stringstream ssCmd;
    ssCmd << "mpv --no-video "
          << track->getUrl()
          << " --input-ipc-server=/tmp/mpvsocket"
          << " 2>&1";
    std::shared_ptr<FILE> pipe(popen(ssCmd.str().c_str(), "r"));
    nowPlaying = true;
}

void YoutubePlaylist::stopPlayback() {
    std::string command("echo '{ \"command\": [\"stop\"] }' | socat - /tmp/mpvsocket");
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    nowPlaying = false;
}

void YoutubePlaylist::playList() {
    //Resume if paused
    if(isPaused())
    {
        std::string command("echo '{ \"command\": [\"set_property\", \"pause\", false] }' | socat - /tmp/mpvsocket");
        std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
        nowPaused = false;
        nowPlaying = true;
        return;
    }
    
    //If not paused, start playing
    std::stringstream ssCmd;
    ssCmd << "mpv --no-video " << playlistUrl << " --input-ipc-server=/tmp/mpvsocket ";
    if(playbackMode == Shuffle)
    {
        ssCmd << "--shuffle";
    }
    else if(playbackMode == Repeat)
    {
        ssCmd << "--loop-playlist";
    }
    else if(playbackMode == ShuffleAndRepeat)
    {
        ssCmd << "--shuffle --loop-playlist";
    }
    ssCmd << " &";
    
    std::string cmd = ssCmd.str();
    system(cmd.c_str());
    
    nowPlaying = true;
}
void YoutubePlaylist::pausePlayback() {
    std::string command("echo '{ \"command\": [\"set_property\", \"pause\", true] }' | socat - /tmp/mpvsocket");
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    nowPaused = true;
}

Json::Value YoutubePlaylist::getJson() {
    Json::Value playlist;
    playlist["name"] = name;
    playlist["id"] = listId;
    playlist["url"] = playlistUrl;
    
    if(isPlaying()) {
        playlist["playing"] = "true";
    }
    
    Json::Value tracksJson;
    int i = 0;
    for(std::shared_ptr<Track> track : tracks)
    {
        tracksJson[i] = track->getJson();
        ++i;
    }
    playlist["tracks"] = tracksJson;
    playlist["type"] = "YouTube";
    return playlist;
}
