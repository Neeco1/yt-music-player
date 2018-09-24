#include "youtube/YoutubeHandler.h"
#include "youtube/YoutubePlaylist.h"
#include <string>
#include <thread>
#include <iostream>
#include <sstream>
#include <memory>
#include <atomic>
#include "Utils.h"
#include "jsoncpp/json/json.h"
#include "EventBus/EventBus.hpp"
#include "events/NewPlaylistReadyEvent.h"
#include "events/NewPlaylistFetchFailedEvent.h"

void YoutubeHandler::fetchPlaylistInfo(std::string playlistId, std::string name) {
    try {
        std::stringstream command;
        command << "youtube-dl " << playlistId << " -J -i ";
        std::string cmdStr = command.str();
        std::string result = Utils::execCommand(cmdStr);
        
        //Parse the JSON
        Json::Value jsonData;
        Json::Reader reader;
        bool success = reader.parse(result.c_str(), jsonData);
        if(!success || !jsonData.isMember("entries"))
        {
            //Publish failure event
            NewPlaylistFetchFailedEvent e(*this, playlistId);
            EventBus::FireEvent(e);
            return;
        }
        Json::Value trackEntries = jsonData["entries"];
        
        //Create a new YT Playlist object
        std::string listUrl = "https://youtube.com/playlist?list=" + playlistId;
        auto ytpl = std::make_shared<YoutubePlaylist>(listUrl);
        ytpl->setListId(playlistId);
        //Set the name
        if(name.compare("") == 0)
        {
            ytpl->setName(jsonData["title"].asString());
        }
        else
        {
            ytpl->setName(name);
        }
        
        for(auto track : trackEntries)
        {
            auto curTrack = std::make_shared<Track>(track["id"].asString());
            std::string webpageUrl = track["webpage_url"].asString();
            if(webpageUrl.compare("") == 0)
            {
                continue;
            }
            curTrack->setUrl(webpageUrl);
            curTrack->setName(track["title"].asString());
            curTrack->setDuration(Utils::secondsToTrackDuration(track["duration"].asUInt()));
            curTrack->setThumbUrl(track["thumbnail"].asString());
            ytpl->addTrack(curTrack);
        }
        
        //Publish new event that playlist is ready
        NewPlaylistReadyEvent e(*this, ytpl);
        EventBus::FireEvent(e);
        
    }
    catch(...)
    {
        //Publish failure event
        NewPlaylistFetchFailedEvent e(*this, playlistId);
        EventBus::FireEvent(e);
    }
}

void YoutubeHandler::newPlaylist(std::string & playlistId, std::string & name) {
    std::thread t(&YoutubeHandler::fetchPlaylistInfo, this, playlistId, name);
    t.detach();
}

void YoutubeHandler::fetchTrackInfo(std::string & url) {
    
}
