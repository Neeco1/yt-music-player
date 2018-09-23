#ifndef TRACK_H__
#define TRACK_H__

#include <string>
#include "Types.h"
#include "jsoncpp/json/json.h"

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
