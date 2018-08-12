#ifndef WEBSOCKET_PLAYER_H__
#define WEBSOCKET_PLAYER_H__

#include <thread>
#include "MusicPlayer.h"
#include "simple-websocket-server/server_ws.hpp"

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

class WebSocketPlayer {
private:
    int port;
    
    WsServer socketSrv;
    MusicPlayer player;
    
public:
    WebSocketPlayer(int port);
    void initConnectionEndpoints();
    
    std::thread start();
    
};

#endif
