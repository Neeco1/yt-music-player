#include "Utils.h"
#include <array>
#include <iostream>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <string>
#include <time.h>
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>

std::string Utils::execCommand(std::string & cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        return result;
    }
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
    }
    return result;
}

TrackDuration Utils::secondsToTrackDuration(unsigned int seconds) {
    TrackDuration newDuration;
    newDuration.seconds = seconds % 60;
    newDuration.minutes = seconds / 60;
    newDuration.hours = seconds / 3600;
    return newDuration;
}

std::string Utils::getTimeString() {
    time_t rawtime;
    time (&rawtime);
    struct tm * timeinfo = localtime (&rawtime);
    char buffer [80];
    strftime (buffer, 80, "%Y-%m-%d_%H:%M:%S", timeinfo);
    return std::string(buffer);
}

bool Utils::checkDirectory() {
    std::string pathname = std::string(getenv("HOME")) + "/websocketPlayer";
    struct stat info;
    //Check if directory exists
    if(stat(pathname.c_str(), &info) != 0) // "stat != 0" means "does not exist"
    {
        //Create directory
        #if defined(_WIN32)
        if(_mkdir(pathname.c_str()) != 0)
        #else
        if(mkdir(pathname.c_str(), 0733) != 0)
        #endif
        {
            exit(EXIT_FAILURE);
        }
    }
    return true;
}

std::string Utils::jsonToString(const Json::Value & json) {
    std::ostringstream outputString;
    outputString << json;
    return outputString.str();
}

Json::Value Utils::readJsonFromFile(std::string pathname) {
    //Check if json file exists
    std::string jsonStr;
    struct stat info;
    if(stat(pathname.c_str(), &info) == 0) // "stat == 0" means "does exist"
    {
        //Read file
        std::ifstream inFile(pathname);
        if (!inFile)
        {
            std::cerr << "Unable to open file \"playlists.json\"!" << std::endl;
            exit(1);
        }
        //Reserve memory in string
        inFile.seekg(0, std::ios::end);
        jsonStr.reserve(inFile.tellg());
        inFile.seekg(0, std::ios::beg);
        //Read data into string
        jsonStr.assign((std::istreambuf_iterator<char>(inFile)),
            std::istreambuf_iterator<char>());
        inFile.close();
    }
    if(jsonStr.length() == 0) { return Json::Value(); }
    
    //Parse the json
    Json::Value jsonData;
    Json::Reader reader;
    bool success = reader.parse(jsonStr.c_str(), jsonData);
    if (!success) { std::remove(pathname.c_str()); return Json::Value(); }
    return jsonData;
}

bool Utils::writeJsonToFile(Json::Value json, std::string pathname) {
    std::string jsonStr;
    std::ofstream jsonFile;
    jsonFile.open(pathname, std::ios::out | std::ios::trunc);
    if(jsonFile.is_open())
    {
        jsonFile << jsonToString(json);
        jsonFile.close();
        return true;
    }
    return false;
}

Json::Value Utils::playlistsToJson(std::vector<std::shared_ptr<Playlist>> lists) {
    Json::Value listsJson;
    for(auto listPtr : lists)
    {
        listsJson[listPtr->getListId()] = listPtr->getJson();
    }
    return listsJson;
}
