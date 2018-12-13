#include "MPV_Listener.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring> //for memset
#include <chrono>
#include <iostream>
#include "jsoncpp/json/json.h"
#include "EventBus/EventBus.hpp"
#include "events/FileStartPlayingEvent.h"
#include "events/FileEndPlayingEvent.h"
#include "events/PlaybackTimeUpdatedEvent.h"

//public and static
void MPV_Listener::startMPVListener() {
    //Check if we need to restart the thread
    auto status = getInstance().listenerFuture.wait_for(std::chrono::milliseconds(0));
    if(status == std::future_status::ready)
    {
        getInstance().stopListener = false;
        getInstance().startListenerThread();
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
            std::cerr << "Socket connection to mpv failed!" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        setSocketTimeout(5);
        //Send command to listen to playback time
        MPV_Listener::observeProperty("playback-time");
        while(!stopListener)
        {
            handleData();
        }
    });
}

void MPV_Listener::setSocketTimeout(int seconds) {
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    setsockopt(sockDescriptor, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
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

    if (connect(sockDescriptor, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        return false;
    }
    return true;
}

bool MPV_Listener::handleData() {
    char buf[1024];
    int rxByteCount;
    
    //Read at max 1024 bytes from socket
    while( (rxByteCount = read(sockDescriptor, buf, 1024)) > 0 )
    {
        std::string nextMsg;
        //Parse the data (read until "new line" character)
        for (int i=0; i<rxByteCount; ++i)
        {
            char c = buf[i];
            if (c == '\n')
            {
                if (nextMsg.length() > 0)
                {
                    Json::Value fullJson;
                    std::string evt = parseEventString(nextMsg, fullJson);
                    handleEvent(evt, fullJson);
                    
                    //Empty "nextMsg" again and start over
                    nextMsg = "";
                }
            }
            else nextMsg += c;
        }
        return true;
    }
    
    return false;
}

std::string MPV_Listener::parseEventString(std::string evtStr, Json::Value & fullJson) {
    Json::Reader reader;
    bool success = reader.parse(evtStr.c_str(), fullJson);

    if (!success || !fullJson.isMember("event")) { return ""; }
    return fullJson["event"].asString();
}

void MPV_Listener::handleEvent(std::string mpvEvt, Json::Value & fullJson) {
    if(mpvEvt.compare("") == 0) { return; }

    //Observe changed properties
    if(mpvEvt.compare("property-change") == 0)
    {
        //Playback time updated
        if(fullJson["name"].asString().compare("playback-time") == 0)
        {
            //Parse as int (because time from mpv is sometimes negative)
            int time = fullJson["data"].asInt();
            unsigned int newTime = 0;
            if(time > 0)
            {
                newTime = fullJson["data"].asUInt();
            }
            //Publish new event that playback time was updated
            PlaybackTimeUpdatedEvent e(*this, newTime);
            EventBus::FireEvent(e);
        }
        return;
    }
    
    //File ended playing
    if(mpvEvt.compare("end-file") == 0)// || mpvEvt.compare("idle") == 0)
    {
        //Publish new event that playback ended
        FileEndPlayingEvent e(*this);
        EventBus::FireEvent(e);
        return;
    }
    if((mpvEvt.compare("start-file") == 0) || (mpvEvt.compare("playback-restart") == 0))
    {
        FileStartPlayingEvent e(*this);
        EventBus::FireEvent(e);
        return;
    }
    if(mpvEvt.compare("pause") == 0) { }
}

void MPV_Listener::stop() {
    stopListener = true;
}

void MPV_Listener::observeProperty(std::string property) {
    //Check if listener thread is active
    auto status = getInstance().listenerFuture.wait_for(std::chrono::milliseconds(0));
    if(status != std::future_status::ready)
    {
        getInstance().sendCommand("{ \"command\": [\"observe_property\", 1, \""+property+"\"] } \n");
    }
}

void MPV_Listener::sendCommand(std::string cmd) {
    int result = write(sockDescriptor, cmd.c_str(), cmd.length());
    if(result < 0)
    {
        std::cerr << "Error sending command '" << cmd << "' to mpv via socket." << std::endl;
    }
}

