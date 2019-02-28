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
: Playlist()
{
    this->playlistUrl = playlistUrl;
}

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

Json::Value YoutubePlaylist::getJson() {
    Json::Value playlist;
    playlist["name"] = name;
    playlist["id"] = listId;
    playlist["url"] = playlistUrl;
    
    Json::Value tracksJson;
    int i = 0;
    std::vector<std::shared_ptr<Track>> * listPtr;
    listPtr = &tracks;

    for(auto trackIt = listPtr->begin() ; trackIt != listPtr->end() ; ++trackIt)
    {
        tracksJson[i] = (*trackIt)->getJson();
        ++i;
    }
    playlist["tracks"] = tracksJson;
    playlist["type"] = "YouTube";
    if(isPlaying())
    {
        playlist["playing"] = true;
    }
    return playlist;
}
