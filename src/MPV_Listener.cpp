#include "MPV_Listener.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring> //for memset
#include <chrono>
#include <iostream>
#include "jsoncpp/json/json.h"
#include "EventBus/EventBus.hpp"
#include "events/FileEndPlayingEvent.h"

//public and static
void MPV_Listener::startMPVListener() {
    static MPV_Listener instance;
    //Check if we need to restart the thread
    auto status = instance.listenerFuture.wait_for(std::chrono::milliseconds(0));
    if(status == std::future_status::ready)
    {
        instance.stopListener = false;
        instance.startListenerThread();
    }
}

//private
MPV_Listener::MPV_Listener() {
    stopListener = false;
    startListenerThread();
}

//private
void MPV_Listener::startListenerThread() {
    listenerFuture = std::async(std::launch::async, [this] {
        while(!connectSocket())
        {
            if(stopListener) { return; }
            std::cout << "Socket Connection failed!" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
        std::cout << "MPV Listener connected." << std::endl;
        while(!stopListener)
        {
            handleData();
        }
    });
}

bool MPV_Listener::connectSocket() {
    struct sockaddr_un addr;
    
    if ( (sockDescriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        return false;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, mpvSocketUrl.c_str(), sizeof(addr.sun_path)-1);

    if (!connect(sockDescriptor, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        return false;
    }
    return true;
}

bool MPV_Listener::handleData() {
    char buf[100];
    int rxByteCount;
    
    while( (rxByteCount = read(sockDescriptor, buf, sizeof(buf))) > 0 )
    {
        std::string mpvEvt = parseEventString(std::string(buf));
        handleEvent(mpvEvt);
        return true;
    }
    
    return false;
}

std::string MPV_Listener::parseEventString(std::string data) {
    Json::Value jsonData;
    Json::Reader reader;
    bool success = reader.parse(data.c_str(), jsonData);

    if (!success || !jsonData.isMember("event")) { return ""; }
    return jsonData["event"].asString();
}

void MPV_Listener::handleEvent(std::string mpvEvt) {
    //std::cout << "MPV event: " << mpvEvt << std::endl;
    if(mpvEvt.compare("") == 0) { return; }
    //File ended playing
    if(mpvEvt.compare("end-file") == 0 || mpvEvt.compare("idle") == 0)
    {
        //Publish new event that playback stopped
        FileEndPlayingEvent e(*this);
        EventBus::FireEvent(e);
        return;
    }
    if(mpvEvt.compare("start-file") == 0) { }
    if(mpvEvt.compare("pause") == 0) { }
}

void MPV_Listener::stop() {
    stopListener = true;
}

