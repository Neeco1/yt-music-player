#include "WebSocketPlayer.h"
#include <iostream>
#include <thread>
#include "simple-websocket-server/server_ws.hpp"

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

void WebSocketPlayer::initConnectionEndpoints() {
    
    auto &test = socketSrv.endpoint["^/test/?$"];
    test.on_message = [](ConnectionPtr connection, InMessagePtr in_message)
    {
        auto out_message = in_message->string();

        std::cout << "Server: Message received: \""
                  << out_message << "\" from "
                  << connection.get()
                  << std::endl;

        std::cout << "Server: Sending message \""
                  << out_message
                  << "\" to "
                  << connection.get()
                  << std::endl;

        // connection->send is an asynchronous function
        connection->send(out_message, [](const SimpleWeb::error_code &ec)
        {
            if(ec)
            {
                std::cout << "Server: Error sending message. "
                          << "Error: " << ec
                          << ", error message: "
                          << ec.message() << std::endl;
            }
        });
    };
}
