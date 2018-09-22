#ifndef TRACK_H__
#define TRACK_H__

#include <string>
#include "Types.h"

class Track {
using string = std::string;

private:
    string name;
    string url;
    string trackId;
    TrackDuration duration;
    string thumbUrl;
    
public:
    Track(const string & trackId)
        : trackId(trackId) {}
    
    


    /** Getters and Setters **/
    string getName() const {
        return name;
    }
    void setName(const string & name) {
        this->name = name;
    }
    
    string getUrl() const {
        return url;
    }
    void setUrl(const string & url) {
        this->url = url;
    }
    
    string getTrackId() const {
        return trackId;
    }
    void setTrackId(const string & trackId) {
        this->trackId = trackId;
    }
    
    TrackDuration getDuration() const {
        return duration;
    }
    void setDuration(const TrackDuration & duration) {
        this->duration = duration;
    }
    
    void setThumbUrl(const string & thumbUrl) {
        this->thumbUrl = thumbUrl;
    }
    string getThumbUrl() {
        return thumbUrl;
    }
};


#endif
