#include "BasicPlaylist.h"
#include "jsoncpp/json/json.h"

BasicPlaylist::BasicPlaylist()
: Playlist()
{ }

BasicPlaylist::BasicPlaylist(Json::Value json)
: Playlist()
{
    setName(json["name"].asString());
    for(Json::Value track : json["tracks"])
    {
        addTrack(std::make_shared<Track>(track));
    }
}

Json::Value BasicPlaylist::getJson() {
    Json::Value playlist;
    playlist["name"] = name;
    
    Json::Value tracksJson;
    int i = 0;
    for(std::shared_ptr<Track> track : tracks)
    {
        tracksJson[i] = track->getJson();
        ++i;
    }
    playlist["tracks"] = tracksJson;
    playlist["type"] = "Basic";
    if(isPlaying())
    {
        playlist["playing"] = true;
    }
    return playlist;
}
