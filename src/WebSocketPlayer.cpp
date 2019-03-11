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
    handlerRegs.push_back(EventBus::AddHandler<PlaylistFetchStartEvent>(*this));
    handlerRegs.push_back(EventBus::AddHandler<FileStartPlayingEvent>(*this));
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
            std::cerr << "Error occurred while running the socket server..." << std::endl;
            exit(1);
        }
    }));
}

void WebSocketPlayer::initCommandEndpoint() {
    commandEndpoint.on_open = [this](ConnectionPtr connection)
    {
        auto pbState = player.getPlaybackState();
        if(pbState == Playing || pbState == Paused) 
        {
            Json::Value responseJson;
            responseJson["error"] = 0;
            responseJson["data"]["status"] = "playback_started";
            std::string responseStr = Utils::jsonToString(responseJson);
            connection->send(responseStr, [](const SimpleWeb::error_code &ec) { });
        }
    };

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
        else if(command.compare("get_single_playlist") == 0)
        {
        	/**
			 * Data format: { "playlist_id" : "some_valid_playlist_id" }
			 */
        	if(data.isMember("playlist_id"))
        	{
				try
				{
					auto playlistPtr = player.getPlaylist(data["playlist_id"].asString());
					responseJson["error"] = 0;
					responseJson["data"]["status"] = "get_single_playlist";
					responseJson["data"]["playlist"] = playlistPtr->getJson();
				}
				catch(std::out_of_range & ex)
				{
					responseJson["error"] = 1;
					responseJson["data"]["errorCode"] = "PLAYLIST_NOT_FOUND";
				}
        	}
        	else
        	{
        		responseJson["error"] = 1;
				responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
        	}
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
        else if(command.compare("update_playlists") == 0)
        {
            responseJson = updatePlaylists();
            sendToAll(responseJson);
            return;
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
        else if(command.compare("play_track_from_list") == 0)
        {
            /**
             * Data format: { "playlist_id" : "some_valid_playlist_id",
             *                "track_index" : <int> }
             */
            if(data.isMember("playlist_id") && data.isMember("track_index"))
            {
                //Select playlist
                responseJson = selectPlaylist(data["playlist_id"].asString());
                sendToAll(responseJson);
                //Play track with given index
                player.playTrackFromCurrentListWithIndex(data["track_index"].asUInt());
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
        else if(command.compare("nextTrack") == 0)
        {
        	responseJson = nextTrack();
        }
        else if(command.compare("previousTrack") == 0)
        {
            responseJson = previousTrack();
        }
        else if(command.compare("setPlaybackTime") == 0)
        {
            /**
             * Data format: { "time" : some_float }
             */
            if(data.isMember("time"))
            {
                responseJson = setPlaybackTime(data["time"].asString());
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
            }
        }
        else if(command.compare("play_media_from_url") == 0)
        {
             /**
             * Data format: { "url" : some_url }
             */
            if(data.isMember("url"))
            {
                responseJson = playMediaFromUrl(data["url"].asString());
            }
            else
            {
                responseJson["error"] = 1;
                responseJson["data"]["errorCode"] = "MALFORMED_REQUEST";
            }
        }
        else if(command.compare("update_yt_dl") == 0)
        {
            Utils::updateYoutubeDl();
            responseJson["error"] = 0;
            responseJson["data"]["status"] = "null";
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

Json::Value WebSocketPlayer::nextTrack() {
    Json::Value responseJson;
    if(player.playNext())
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "next_track";
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::previousTrack() {
    Json::Value responseJson;
    if(player.playPrevious())
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "previous_track";
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
    std::string result = MusicPlayer::addPlaylistFromUrl(url, name);
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

Json::Value WebSocketPlayer::setPlaybackMode(const std::string & mode) {
    Json::Value responseJson;
    bool success = false;

    static int run = 0;

    //Find out the given playback mode
    if(mode.compare("Shuffle") == 0)
    {
    	success = player.setPlaybackMode(PlaybackMode::Shuffle);
    }
    else if(mode.compare("Repeat") == 0)
    {
        success = player.setPlaybackMode(PlaybackMode::Repeat);
    }
    else if(mode.compare("Normal") == 0)
    {
        success = player.setPlaybackMode(PlaybackMode::Normal);
    }
    ++run;
    
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
    pbInfo["track_duration"] = info.duration;
    pbInfo["track_playback_time"] = info.playbackTime;
    
    std::string mode;
    switch(info.playbackMode)
    {
    	case Shuffle:
    		mode = "Shuffle";
    		break;

        case Repeat:
            mode = "Repeat";
            break;
            
        case Normal:
        default:
            mode = "Normal";
            break;
    }
    pbInfo["playback_mode"] = mode;
    
    Json::Value responseJson;
    responseJson["error"] = 0;
    responseJson["data"]["status"] = "playback_info";
    responseJson["data"]["playback_info"] = pbInfo;
    return responseJson;
}

Json::Value WebSocketPlayer::updatePlaylists() {
    Json::Value responseJson;
    if(player.updatePlaylists())
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "update_playlists";
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["status"] = "error";
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::setPlaybackTime(std::string time) {
    Json::Value responseJson;
    if(player.setPlaybackTime(time))
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playback_time_set";
        responseJson["data"]["time"] = time;
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["status"] = "error";
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
    return responseJson;
}

Json::Value WebSocketPlayer::playMediaFromUrl(std::string url) {
    Json::Value responseJson;
    if(player.playMediaFromUrl(url))
    {
        responseJson["error"] = 0;
        responseJson["data"]["status"] = "playback_started";
        responseJson["data"]["url"] = url;
    }
    else
    {
        responseJson["error"] = 1;
        responseJson["data"]["status"] = "error";
        responseJson["data"]["errorCode"] = "PLAYER_ERROR";
    }
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
    //Notify all clients that the fetch of data failed
    Json::Value responseJson;
    responseJson["error"] = 1;
    responseJson["data"]["status"] = "playlist_fetch_failed";
    responseJson["data"]["playlist_id"] = e.getPlaylistId();
    sendToAll(responseJson);
}

void WebSocketPlayer::onEvent(PlaylistFetchStartEvent & e) {
    //Notify all clients that a playlist is being fetched
    Json::Value responseJson;
    responseJson["error"] = 0;
    responseJson["data"]["status"] = "playlist_fetch_start";
    responseJson["data"]["playlist_id"] = e.getPlaylistId();
    responseJson["data"]["playlist_name"] = e.getPlaylistName();
    sendToAll(responseJson);
}

void WebSocketPlayer::onEvent(FileStartPlayingEvent & e) {
    Json::Value msgJson;
    msgJson["error"] = 0;
    msgJson["data"]["status"] = "playback_started";
    sendToAll(msgJson);
}
