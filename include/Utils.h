#ifndef MUSIC_PLAYER_UTILS_H__
#define MUSIC_PLAYER_UTILS_H__

#include <string>
#include "Types.h"

class Utils {
public:
    static std::string execCommand(std::string & cmd);
    static TrackDuration secondsToTrackDuration(unsigned int seconds);
};

#endif
