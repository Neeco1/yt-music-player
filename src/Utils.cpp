#include "Utils.h"
#include <array>
#include <iostream>
#include <string>
#include <memory>

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
