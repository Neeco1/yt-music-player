#ifndef MPV_CONTROLLER_H__
#define MPV_CONTROLLER_H__

#include <string>
#include <map>
#include "Types.h"

static std::string mpvSocketUrl = "/tmp/mpvsocket";

class MPV_Controller {
private:
    static bool sendStringCommand(std::string command);

public:
    typedef std::map<std::string, std::string> CommandMap;
    
    static CommandMap commands;
    
    static void startMPVIdle();

    static std::string getProperty(std::string command);
    static int getPropertyAsInt(std::string command);
    static bool sendCommandFromMap(std::string command);
    static bool sendCommand(std::string command);
    static bool setProperty(std::string property, std::string data);
    
    static void playMedia(std::string mediaUrl);
};

#endif
