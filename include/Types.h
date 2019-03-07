#ifndef TYPES_H__
#define TYPES_H__

#include <string>

struct TrackDuration {
    unsigned int hours;
    unsigned int minutes;
    unsigned int seconds;
} ;

enum PlaybackMode {
    Normal, Shuffle, Repeat
};

enum SupportedSources {
    FromDisk, YouTube
};

enum PlaybackState {
    Paused, Stopped, Playing
};

struct PlaybackInfo {
    std::string state;
    std::string title;
    std::string trackId;
    unsigned int playbackTime;
    unsigned int duration;
    PlaybackMode playbackMode;
    std::string thumbUrl;
    
    PlaybackInfo()
    : state(""), title(""), trackId(""), playbackTime(0), duration(0),
      playbackMode(Normal), thumbUrl("") {}
};




#endif
