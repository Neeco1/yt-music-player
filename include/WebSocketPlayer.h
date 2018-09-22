#ifndef WEBSOCKET_PLAYER_H__
#define WEBSOCKET_PLAYER_H__

#include <thread>
#include "MusicPlayer.h"
#include "simple-websocket-server/server_ws.hpp"
#include "jsoncpp/json/json.h"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class WebSocketPlayer {
private:
    int port;
    
    WsServer socketSrv;
    MusicPlayer player;
    
public:
    WebSocketPlayer(int port);
    
    std::string jsonToResponseString(const Json::Value & json);
    
    void initStopEndpoint();
    void initPlayEndpoint();
    void initSetVolumeEndpoint();
    void initGetPlaylistsEndpoint();
    void initConnectionEndpoints();
    void initAddPlaylistEndpoint();
    
    std::thread start();
    
};

#endif
