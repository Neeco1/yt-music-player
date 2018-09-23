#ifndef TYPES_H__
#define TYPES_H__

#include <string>

struct TrackDuration {
    unsigned int hours;
    unsigned int minutes;
    unsigned int seconds;
} ;

enum PlaybackMode {
    Normal, Shuffle, Repeat, ShuffleAndRepeat
};

enum SupportedSources {
    FromDisk, YouTube
};

struct PlaybackInfo {
    std::string state;
    std::string title;
    std::string trackId;
    unsigned int playbackTime;
    unsigned int duration;
    PlaybackMode playbackMode;
    std::string thumbUrl;
};




#endif
