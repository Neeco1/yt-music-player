#include <sstream>
#include "MPV_Controller.h"
#include "jsoncpp/json/json.h"
#include "Utils.h"
#include <iostream>

MPV_Controller::CommandMap MPV_Controller::commands  = {
    { "mpvSocketUrl", mpvSocketUrl },
    { "cmdGetMediaTitle", "echo '{ \"command\": "
        "[\"get_property\", \"media-title\"] }' | socat - " + mpvSocketUrl },
    { "cmdGetPlaybackTime", "echo '{ \"command\": "
        "[\"get_property\", \"playback-time\"] }' | socat - " + mpvSocketUrl },
    { "cmdGetDuration", "echo '{ \"command\": "
        "[\"get_property\", \"duration\"] }' | socat - " + mpvSocketUrl },
    { "cmdGetRemainingTime", "echo '{ \"command\": "
        "[\"get_property\", \"playtime-remaining\"] }' | socat - " + mpvSocketUrl },
    { "cmdSetLoopFile", "echo '{ \"command\": "
        "[\"set_property\", \"loop-file\", true] }' | socat - " + mpvSocketUrl },
    { "cmdNoLoopFile", "echo '{ \"command\": "
        "[\"set_property\", \"loop-file\", false] }' | socat - " + mpvSocketUrl },
    { "cmdStopPlayback", "echo '{ \"command\": "
        "[\"stop\"] }' | socat - " + mpvSocketUrl },
    { "cmdPausePlayback", "echo '{ \"command\": "
        "[\"set_property\", \"pause\", true] }' | socat - " + mpvSocketUrl },
    { "cmdUnpausePlayback", "echo '{ \"command\": "
        "[\"set_property\", \"pause\", false] }' | socat - " + mpvSocketUrl }
};

void MPV_Controller::startMPVIdle() {
    std::stringstream ssCmd;
    ssCmd << "mpv --no-video --idle --input-ipc-server=" << mpvSocketUrl << " &";
    std::string cmd = ssCmd.str();
    system(cmd.c_str());
}

std::string MPV_Controller::getProperty(std::string command) {
    std::string jsonResult = Utils::execCommand(commands.at(command));
    //Parse data
    Json::Value j;
    Json::Reader reader;
    try
    {
        bool success = reader.parse(jsonResult.c_str(), j);
        if (!success) { return ""; }
        return j.get("data", "").asString();
    }
    catch(...)
    {
        return "";
    }
}

int MPV_Controller::getPropertyAsInt(std::string command) {
    std::string value = getProperty(command);
    if(value.compare("") == 0)
    {
        return 0;
    }
    return std::stoi(value);
}

bool MPV_Controller::setProperty(std::string command) {
    std::string jsonResult = Utils::execCommand(commands.at(command));
    //Parse data
    Json::Value j;
    Json::Reader reader;
    try
    {
        bool success = reader.parse(jsonResult.c_str(), j);
        if (!success) { return false; }
        std::string result = j.get("error", "").asString();
        return (result.compare("success") == 0);
    }
    catch(...)
    {
        return false;
    }
}

void MPV_Controller::playMedia(std::string mediaUrl) {
    std::stringstream ssCmd;
    ssCmd << "echo '{ \"command\": [\"loadfile\", \""
          << mediaUrl
          << "\"] }' | socat - /tmp/mpvsocket";
    std::string cmd = ssCmd.str();
    system(cmd.c_str());
}
