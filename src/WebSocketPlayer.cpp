#include "WebSocketPlayer.h"
#include <iostream>
#include <thread>
#include <string>
#include "simple-websocket-server/server_ws.hpp"
#include "jsoncpp/json/json.h"
#include "EventBus/EventBus.hpp"

typedef std::shared_ptr<WsServer::Connection> ConnectionPtr;
typedef std::shared_ptr<WsServer::InMessage> InMessagePtr;

WebSocketPlayer::WebSocketPlayer(int port)
    : port(port), commandEndpoint(socketSrv.endpoint["^/action/?$"])
{
    socketSrv.config.port = 8080;
    initCommandEndpoint();
    handlerRegs.push_back(EventBus::AddHandler<NewPlaylistReadyEvent>(*this));
    handlerRegs.push_back(EventBus::AddHandler<NewPlaylistFetchFailedEvent>(*this));
}

WebSocketPlayer::~WebSocketPlayer() {
    for(auto handlerReg : handlerRegs)
    {
        if(handlerReg != nullptr)
        {
            handlerReg->removeHandler();
            delete handlerReg;
        }
    }
}

std::thread WebSocketPlayer::startThread() {
    return std::move(std::thread([this]() {
        // Start WS-server
        try
        {
            socketSrv.start();
        }
        catch(...)
        {
            std::cerr << "Failed to start socket server..." << std::endl;
            exit(1);
        }
    }));
}

void WebSocketPlayer::initCommandEndpoint() {
    commandEndpoint.on_message = [this](ConnectionPtr connection, InMessagePtr in_message)
    {
        //Try to parse data
        Json::Value clientRequest;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(in_message->string().c_str(), clientRequest);
        
        Json::Value responseJson;
        if (!parsingSuccessful || !clientRequest.isMember("cmd")
            || !clientRequest.isMember("data"))
        {
            responseJson["error"] = 1;
            responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
            std::string responseStr = Utils::jsonToString(responseJson);
            // connection->send is an asynchronous function
            connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });
            return;
        }
        
        std::string command = clientRequest["cmd"].asString();
        Json::Value data = clientRequest["data"];
        
        if(command.compare("stop") == 0)
        {
            responseJson = stop();
            sendToAll(responseJson);
            return;
        }
        else if(command.compare("play") == 0)
        {
            responseJson = start();
        }
        else if(command.compare("pause") == 0)
        {
            responseJson = pause();
            sendToAll(responseJson);
            return;
        }
        else if(command.compare("set_volume") == 0)
        {
            /**
             * Data format: { "volume" : 50 }
             */
            if(data.isMember("volume"))
            {
                unsigned int percentage = data["volume"].asUInt();
                responseJson = setVolume(percentage);
                sendToAll(responseJson);
                return;
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
            }
        }
        else if(command.compare("get_volume") == 0)
        {
            //TODO
        }
        else if(command.compare("get_playlists") == 0)
        {
            responseJson["error"] = 0;
            responseJson["data"]["status"] = "playlists";
            responseJson["data"]["playlists"] = Utils::playlistsToJson(player.getPlaylists());
        }
        else if(command.compare("add_playlist") == 0)
        {
            /**
             * Data format: { "playlist_url" : "some_valid_playlist_url", "name" : "some_name" }
             */
            if(data.isMember("playlist_url") && data.isMember("name"))
            {
                responseJson = addPlaylist(
                    data["playlist_url"].asString(),
                    data["name"].asString());
                sendToAll(responseJson);
                return;
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
            }
        }
        else if(command.compare("select_playlist") == 0)
        {
            /**
             * Data format: { "playlist_id" : "some_valid_playlist_id" }
             */
            if(data.isMember("playlist_id"))
            {
                responseJson = selectPlaylist(data["playlist_id"].asString());
                sendToAll(responseJson);
                return;
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
            }
        }
        else if(command.compare("set_playback_mode") == 0)
        {
            /**
             * Data format: { "mode" : "some_valid_mode" }
             */
            if(data.isMember("mode"))
            {
                responseJson = setPlaybackMode(data["mode"].asString());
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
            }
        }
        else if(command.compare("get_playback_info") == 0)
        {
            responseJson = getPlaybackInfo();
        }
        else
        {
            responseJson["error"] = 1;
            responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
        }
        std::string responseStr = Utils::jsonToString(responseJson);
        
        //Send reply to all connections
        connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });
        return;
    };
}

Json::Value WebSocketPlayer::stop() {
    Json::Value responseJson;
    if(player.stopPlayback())
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playback_stopped";
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::start() {
    Json::Value responseJson;
    if(player.startPlayback())
    {
        std::cout << "Playback started!" << std::endl;
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playback_started";
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::pause() {
    Json::Value responseJson;
    if(player.pausePlayback())
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playback_paused";
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::setVolume(unsigned int percentage) {
    Json::Value responseJson;
    if(player.setVolume(percentage))
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "volume_set";
        responseJson["data"]["value"] = "percentage";
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::addPlaylist(const std::string & url, const std::string & name) {
    Json::Value responseJson;
    std::string result = player.addPlaylistFromUrl(url, name);
    if(!result.compare("") == 0)
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playlist_add_in_progress";
        responseJson["data"]["id"] = result;
        responseJson["data"]["name"] = name;
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["status"] = "error";
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::selectPlaylist(std::string playlist_id) {
    Json::Value responseJson;
    if(player.selectPlaylist(playlist_id))
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playlist_selected";
        responseJson["data"]["playlist_id"] = playlist_id;
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["status"] = "error";
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::setPlaybackMode(std::string mode) {
    Json::Value responseJson;
    
    bool success = false;
    //Find out the given playback mode
    if(mode.compare("Shuffle") == 0)
    {
        success = player.setPlaybackMode(Shuffle);
    }
    else if(mode.compare("Normal") == 0)
    {
        success = player.setPlaybackMode(Normal);
    }
    
    //Check if mode was valid and successful
    if(success)
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playback_mode_set";
        responseJson["data"]["mode"] = mode;
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["status"] = "error";
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    
    return responseJson;
}

Json::Value WebSocketPlayer::getPlaybackInfo() {
    PlaybackInfo info = player.getPlaybackInfo();
    Json::Value pbInfo;
    pbInfo["state"] = info.state;
    pbInfo["title"] = info.title;
    pbInfo["track_id"] = info.trackId;
    pbInfo["thumb_url"] = info.thumbUrl;
    
    Json::Value responseJson;
    responseJson["error"] = 0;
    responseJson["data"]["status"] = "playback_info";
    responseJson["data"]["playback_info"] = pbInfo;
    return responseJson;
}

void WebSocketPlayer::sendToAll(Json::Value json) {
    std::string msg = Utils::jsonToString(json);
    auto allConnections = commandEndpoint.get_connections();
    for(auto connection : allConnections)
    {
        connection->send(msg, [](const SimpleWeb::error_code &ec) { });
    }
}

void WebSocketPlayer::onEvent(NewPlaylistReadyEvent & e) {
    auto playlist = e.getPlaylist();
    //Build information string for clients
    Json::Value responseJson;
    responseJson["error"] = 0;
    responseJson["data"]["status"] = "playlist_add_done";
    responseJson["data"]["playlist"] = playlist->getJson();
    sendToAll(responseJson);
}

void WebSocketPlayer::onEvent(NewPlaylistFetchFailedEvent & e) {
    std::cout << "Received fetch failed event..." << std::endl;
    //Notify all clients that the fetch of data failed
    Json::Value responseJson;
    responseJson["error"] = 1;
    responseJson["data"]["status"] = "playlist_fetch_failed";
    responseJson["data"]["playlist_id"] = e.getPlaylistId();
    sendToAll(responseJson);
}