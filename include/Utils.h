#ifndef MUSIC_PLAYER_UTILS_H__
#define MUSIC_PLAYER_UTILS_H__

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "Types.h"
#include "jsoncpp/json/json.h"
#include "Playlist.h"

class Utils {
private:
    static std::mutex jsonFileMutex;
    
public:
    static std::string execCommand(std::string & cmd);
    static TrackDuration secondsToTrackDuration(unsigned int seconds);
    static std::string getTimeString();
    static std::string getWorkingDir();
    
    static bool checkDirectory();
    static std::string jsonToString(const Json::Value & json);
    static bool writeJsonToFile(Json::Value json, std::string pathname);
    static Json::Value readJsonFromFile(std::string pathname);
    static Json::Value playlistsToJson(std::vector<std::shared_ptr<Playlist>> lists);
    
    static void updateYoutubeDl();
};

#endif
