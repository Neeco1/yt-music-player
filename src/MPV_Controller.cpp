#include <sstream>
#include "MPV_Controller.h"
#include "jsoncpp/json/json.h"
#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

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
    ssCmd << "mpv --really-quiet --no-video --idle "
          << "--input-ipc-server=" << mpvSocketUrl
          << " > /dev/null 2>&1 &";
    std::string cmd = ssCmd.str();
    std::system(cmd.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
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

bool MPV_Controller::sendStringCommand(std::string command) {
    std::string jsonResult = Utils::execCommand(command);
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
bool MPV_Controller::sendCommandFromMap(std::string command) {
    return sendStringCommand(commands.at(command));
}

bool MPV_Controller::setProperty(std::string property, std::string data) {
    std::stringstream ssCmd;
    ssCmd << "echo '{ \"command\": [\"set_property\", \""
          << property
          << "\", \""
          << data
          << "\"] }' | socat - "
          << mpvSocketUrl;
    std::string cmd = ssCmd.str();
    return sendStringCommand(cmd);
}

void MPV_Controller::playMedia(std::string mediaUrl) {
    std::stringstream ssCmd;
    ssCmd << "echo '{ \"command\": [\"loadfile\", \""
          << mediaUrl
          << "\"] }' | socat - "
          << mpvSocketUrl;
    std::string cmd = ssCmd.str();
    std::system(cmd.c_str());
}
