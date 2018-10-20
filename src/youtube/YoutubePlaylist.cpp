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
: Playlist(), stopPlaybackFlag(false)
{
    this->playlistUrl = playlistUrl;
}

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

Json::Value YoutubePlaylist::getJson() {
    Json::Value playlist;
    playlist["name"] = name;
    playlist["id"] = listId;
    playlist["url"] = playlistUrl;
    
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
