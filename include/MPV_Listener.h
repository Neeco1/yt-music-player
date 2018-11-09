#ifndef MPV_LISTENER_H__
#define MPV_LISTENER_H__

#include <future>
#include <atomic>
#include <EventBus/Object.hpp>
#include "MPV_Controller.h"
#include "jsoncpp/json/json.h"

class MPV_Listener : public Object {
private:
    //Singleton
    static MPV_Listener & getInstance() {
        static MPV_Listener instance;
        return instance;
    }
    
    MPV_Listener(); //Private constructor
    MPV_Listener(MPV_Listener const&)= delete;
    void operator=(MPV_Listener const&) = delete;
    
    void startListenerThread();
    
    std::future<void> listenerFuture;
    std::atomic<bool> stopListener;
    int sockDescriptor;
    bool socketConnected;
    
    void setSocketTimeout(int seconds);
    std::string parseEventString(std::string evtStr, Json::Value & fullJson);
    void handleEvent(std::string event, Json::Value & fullJson);

public:
    static void startMPVListener();
    static void observeProperty(std::string property);
    
    bool connectSocket();
    bool handleData();
    void stop();
    
    void sendCommand(std::string cmd);
};

#endif
