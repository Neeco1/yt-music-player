#ifndef MPV_LISTENER_H__
#define MPV_LISTENER_H__

#include <future>
#include <atomic>
#include <EventBus/Object.hpp>
#include "MPV_Controller.h"

class MPV_Listener : public Object {
private:
    MPV_Listener();
    MPV_Listener(MPV_Listener const&)= delete;
    void operator=(MPV_Listener const&) = delete;
    
    void startListenerThread();
    
    std::future<void> listenerFuture;
    std::atomic<bool> stopListener;
    int sockDescriptor;
    bool socketConnected;
    
    std::string parseEventString(std::string data);
    void handleEvent(std::string event);

public:
    static void startMPVListener();
    
    bool connectSocket();
    bool handleData();
    void stop();
};

#endif
