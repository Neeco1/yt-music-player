#ifndef WEBSOCKET_PLAYER_H__
#define WEBSOCKET_PLAYER_H__

#include <thread>
#include <string>
#include <vector>
#include <memory>
#include "MusicPlayer.h"
#include "simple-websocket-server/server_ws.hpp"
#include "jsoncpp/json/json.h"

#include "events/NewPlaylistReadyEvent.h"
#include "events/NewPlaylistFetchFailedEvent.h"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using WsEndpoint = SimpleWeb::SocketServer<SimpleWeb::WS>::Endpoint;

class WebSocketPlayer : public EventHandler<NewPlaylistReadyEvent>,
                        public EventHandler<NewPlaylistFetchFailedEvent>
{
private:
    int port;
    
    WsServer socketSrv;
    WsEndpoint & commandEndpoint;
    
    MusicPlayer player;
    
    
    //Handler pointer for NewPlaylistReadyEvent
    std::vector<HandlerRegistration*> handlerRegs;
    
public:
    WebSocketPlayer(int port);
    ~WebSocketPlayer();
    
    void initCommandEndpoint();
    
    Json::Value stop();
    Json::Value start();
    Json::Value pause();
    
    Json::Value setVolume(unsigned int percentage);
    Json::Value addPlaylist(const std::string & url, const std::string & name);
    Json::Value selectPlaylist(std::string playlist_id);
    
    Json::Value setPlaybackMode(std::string mode);
    Json::Value getPlaybackInfo();
    
    std::thread startThread();
    
    void sendToAll(Json::Value json);
    
    virtual void onEvent(NewPlaylistReadyEvent & e) override;
    virtual void onEvent(NewPlaylistFetchFailedEvent & e) override;
    
};

#endif