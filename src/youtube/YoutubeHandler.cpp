#include "youtube/YoutubeHandler.h"
#include "youtube/YoutubePlaylist.h"
#include <string>
#include <thread>
#include <iostream>
#include <sstream>
#include "Utils.h"
#include "jsoncpp/json/json.h"
#include "EventBus/EventBus.hpp"
#include "events/NewPlaylistReadyEvent.h"

void YoutubeHandler::fetchPlaylistInfo(std::string playlistId) {
    try {
        std::stringstream command;
        command << "youtube-dl " << playlistId << " -J";
        std::string cmdStr = command.str();
        std::string result = Utils::execCommand(cmdStr);
    
        //Parse the JSON
        Json::Value jsonData;
        Json::Reader reader;
        bool success = reader.parse(result.c_str(), jsonData);
        if(!success || !jsonData.isMember("entries"))
        {
            //TODO: Publish failure event
            return;
        }
        Json::Value trackEntries = jsonData["entries"];
        
        //Create a new YT Playlist object
        auto ytpl = std::make_shared<YoutubePlaylist>();
        ytpl->setListId(playlistId);
        ytpl->setName(jsonData["title"].asString());
        
        for(auto track : trackEntries)
        {
            Track curTrack(track["id"].asString());
            curTrack.setName(track["title"].asString());
            curTrack.setDuration(Utils::secondsToTrackDuration(track["duration"].asUInt()));
            curTrack.setUrl(track["webpage_url"].asString());
            curTrack.setThumbUrl(track["thumbnail"].asString());
            
            ytpl->addTrack(curTrack);
        }
        
        //Publish new event that playlist is ready
        NewPlaylistReadyEvent e(*this, ytpl);
        EventBus::FireEvent(e);
        
    }
    catch(Json::LogicError & error)
    {
        //TODO: Publish failure event
        std::cout << "Json error occurred!" << std::endl;
    }
}

void YoutubeHandler::newPlaylist(std::string & playlistId) {
    std::thread t(&YoutubeHandler::fetchPlaylistInfo, this, playlistId);
    t.detach();
}

void YoutubeHandler::fetchTrackInfo(std::string & url) {
    
}
