#include "WebSocketPlayer.h"
#include <iostream>
#include <thread>
#include "simple-websocket-server/server_ws.hpp"
#include "jsoncpp/json/json.h"
#include "EventBus/EventBus.hpp"

typedef std::shared_ptr<WsServer::Connection> ConnectionPtr;
typedef std::shared_ptr<WsServer::InMessage> InMessagePtr;

WebSocketPlayer::WebSocketPlayer(int port)
    : port(port)
{
    socketSrv.config.port = 8080;
    initConnectionEndpoints();
}

std::thread WebSocketPlayer::start() {
    return std::move(std::thread([this]() {
        // Start WS-server
        socketSrv.start();
    }));
}

std::string WebSocketPlayer::jsonToResponseString(const Json::Value & json) {
    std::ostringstream outputString;
    outputString << json;
    return outputString.str();
}

void WebSocketPlayer::initConnectionEndpoints() {
    initStopEndpoint();
    initPlayEndpoint();
    initSetVolumeEndpoint();
    initGetPlaylistsEndpoint();
    initAddPlaylistEndpoint();
}

/**
 * Handles the "stop" command from the client.
 * If client sends any data, it is ignored.
 */
void WebSocketPlayer::initStopEndpoint() {
    auto &stopEndpoint = socketSrv.endpoint["^/action/stop/?$"];
    stopEndpoint.on_message = [this](ConnectionPtr connection, InMessagePtr in_message)
    {
        Json::Value responseJson;
        if(player.stopPlaying())
        {
            responseJson["error"] = 0;
            responseJson["data"] = "success";
        }
        else
        {
            responseJson["error"] = 1;
            responseJson["data"]["errorCode"] = "PLAYER_ERROR";
        }
        std::string responseStr = jsonToResponseString(responseJson);
        // connection->send is an asynchronous function
        connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });
    };
    
}

/**
 * Handles the "play" command from the client.
 * If client sends any data, it is ignored.
 */
void WebSocketPlayer::initPlayEndpoint() {
    auto &playEndpoint = socketSrv.endpoint["^/action/play/?$"];
    playEndpoint.on_message = [this](ConnectionPtr connection, InMessagePtr in_message)
    {
        Json::Value responseJson;
        if(player.startPlaying())
        {
            responseJson["error"] = 0;
            responseJson["data"] = "success";
        }
        else
        {
            responseJson["error"] = 1;
            responseJson["data"]["errorCode"] = "PLAYER_ERROR";
        }
        std::string responseStr = jsonToResponseString(responseJson);
        // connection->send is an asynchronous function
        connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });
    };
}


/**
 * Handles the "Set volume" command from the client.
 * Data should look like this:
 * { "volume" : 50 }
 */
void WebSocketPlayer::initSetVolumeEndpoint() {
    auto &setVolEndpoint = socketSrv.endpoint["^/action/volume/?$"];
    setVolEndpoint.on_message = [this](ConnectionPtr connection, InMessagePtr in_message)
    {
        //Try to parse data
        Json::Value clientRequest;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(in_message->string().c_str(), clientRequest);
        
        Json::Value responseJson;
        if (parsingSuccessful)
        {
            if(player.setVolume(clientRequest.get("volume", -1).asInt()))
            {
                responseJson["error"] = 0;
                responseJson["data"] = "success";
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "PLAYER_ERROR";
            }
        }
        else
        {
            responseJson["error"] = 1;
            responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
        }
        std::string responseStr = jsonToResponseString(responseJson);
        
        // connection->send is an asynchronous function
        connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });
    };
}

void WebSocketPlayer::initGetPlaylistsEndpoint() {
    auto &getPlaylistsEndpoint = socketSrv.endpoint["^/playlist/getAll/?$"];
    getPlaylistsEndpoint.on_message = [this](ConnectionPtr connection, InMessagePtr in_message)
    {
        auto playlists = player.getPlaylists();
        
        /*if(player.setVolume(clientRequest.get("volume", -1).asInt()))
        {
            responseJson["error"] = 0;
            responseJson["data"] = "success";
        }
        else
        {
            responseJson["error"] = 1;
            responseJson["data"]["errorCode"] = "PLAYER_ERROR";
        }
        std::string responseStr = jsonToResponseString(responseJson);
        
        // connection->send is an asynchronous function
        connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });*/
    };
}

/**
 * Handles the "Add playlist" command from the client.
 * Data should look like this:
 * { "url" : some_valid_playlist_url }
 */
void WebSocketPlayer::initAddPlaylistEndpoint() {
    auto &addPlaylistEndpoint = socketSrv.endpoint["^/action/add_playlist/?$"];
    addPlaylistEndpoint.on_message = [this](ConnectionPtr connection, InMessagePtr in_message)
    {
        //Try to parse data
        Json::Value clientRequest;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(in_message->string().c_str(), clientRequest);
        
        Json::Value responseJson;
        if (parsingSuccessful)
        {
            std::string url = clientRequest.get("url", "").asString();
            if(player.addPlaylistFromUrl(url))
            {
                responseJson["error"] = 0;
                responseJson["data"] = "success";
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "PLAYER_ERROR";
            }
        }
        else
        {
            responseJson["error"] = 1;
            responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
        }
        std::string responseStr = jsonToResponseString(responseJson);
        
        // connection->send is an asynchronous function
        connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });
    };
}
