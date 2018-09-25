#ifndef TRACK_H__
#define TRACK_H__

#include <string>
#include "Types.h"
#include "jsoncpp/json/json.h"

class Track {
private:
    std::string name;
    std::string url;
    std::string trackId;
    TrackDuration duration;
    std::string thumbUrl;
    
public:
    Track(const std::string & trackId)
        : trackId(trackId) {}
    
    Track(Json::Value & track) {
        setTrackId(track["id"].asString());
        setName(track["name"].asString());
        setUrl(track["url"].asString());
        TrackDuration dur;
        dur.hours = track["duration"]["hours"].asUInt();
        dur.minutes = track["duration"]["minutes"].asUInt();
        dur.seconds = track["duration"]["seconds"].asUInt();
        setDuration(dur);
        setThumbUrl(track["thumbUrl"].asString());
    }

    /** Getters and Setters **/
    std::string getName() const {
        return name;
    }
    void setName(const std::string & name) {
        this->name = name;
    }
    
    std::string getUrl() const {
        return url;
    }
    void setUrl(const std::string & url) {
        this->url = url;
    }
    
    std::string getTrackId() const {
        return trackId;
    }
    void setTrackId(const std::string & trackId) {
        this->trackId = trackId;
    }
    
    TrackDuration getDuration() const {
        return duration;
    }
    int getDurationSeconds() const {
        return duration.hours*3600 + duration.minutes*60 + duration.seconds;
    }
    void setDuration(const TrackDuration & duration) {
        this->duration = duration;
    }
    
    void setThumbUrl(const std::string & thumbUrl) {
        this->thumbUrl = thumbUrl;
    }
    std::string getThumbUrl() {
        return thumbUrl;
    }
    
    Json::Value getJson() {
        Json::Value track;
        track["name"] = getName();
        track["url"] = getUrl();
        track["id"] = getTrackId();
        
        TrackDuration dur = getDuration();
        track["duration"]["h"] = dur.hours;
        track["duration"]["m"] = dur.minutes;
        track["duration"]["s"] = dur.seconds;
        
        track["thumbUrl"] = getThumbUrl();
        return track;
    }
};


#endif
