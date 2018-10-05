#include "youtube/YoutubePlaylist.h"
#include <sstream>
#include <iostream>
#include "jsoncpp/json/json.h"
#include <stdlib.h>
#include <thread>
#include <chrono>
#include "MPV_Controller.h"
#include "events/PlaybackStoppedEvent.h"

YoutubePlaylist::YoutubePlaylist(std::string playlistUrl)
: Playlist(), playlistUrl(playlistUrl), stopPlaybackFlag(false)
{ }

YoutubePlaylist::YoutubePlaylist(Json::Value json)
: Playlist(), stopPlaybackFlag(false)
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
    auto track = tracks[trackIndex];
    MPV_Controller::playMedia(track->getUrl());
    nowPlaying = true;
    stopPlaybackFlag = false;
}

void YoutubePlaylist::stopPlayback() {
    //stopPlaybackFlag = true;
    MPV_Controller::sendCommandFromMap("cmdStopPlayback");
    if(!isPlaying())
    {
        currentTrack = 0;
    }
    nowPlaying = false;
    stopPlaybackFlag = true;
}

void YoutubePlaylist::playList() {
    //Resume if paused
    if(isPaused())
    {
        MPV_Controller::sendCommandFromMap("cmdUnpausePlayback");
        nowPaused = false;
        nowPlaying = true;
        return;
    }
    
    //If not paused, start playing
    playTrack(currentTrack);
    stopPlaybackFlag = false;
    nowPlaying = true;
}
void YoutubePlaylist::pausePlayback() {
    MPV_Controller::sendCommandFromMap("cmdPausePlayback");
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
