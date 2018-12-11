#include "Playlist.h"

#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>
#include <iostream>
#include "Track.h"
#include "Types.h"
#include "jsoncpp/json/json.h"
#include "Utils.h"

Playlist::Playlist() : currentTrack(0), playing(false)
{
    //Initially set timestamped id
    std::stringstream ss;
    ss << "list_" << Utils::getTimeString();
    setListId(ss.str());
    setName("New List");
}

void Playlist::setPlaylistUrl(std::string playlistUrl) {
    this->playlistUrl = playlistUrl;
}

std::string Playlist::getPlaylistUrl() {
    return this->playlistUrl;
}

void Playlist::setListId(const std::string & listId) {
    this->listId = listId;
}
std::string Playlist::getListId() {
    return listId;
}

void Playlist::setName(const std::string & name) {
    this->name = name;
}
const std::string & Playlist::getName() {
    return name;
}

void Playlist::addTrack(const std::shared_ptr<Track> track) {
    tracks.push_back(track);
}

const std::vector<std::shared_ptr<Track>> & Playlist::getAllTracks() const {
    return tracks;
}

const unsigned int Playlist::getTrackCount() const {
    return tracks.size();
}

void Playlist::setPlaying(bool value) {
    playing = value;
}
bool Playlist::isPlaying() {
    return playing;
}

void Playlist::setCurrentTrackNumber(unsigned int trackNumber) {
    unsigned int trackCount = getTrackCount();
    if(trackNumber >= trackCount) {
        trackNumber = trackCount;
    }
    this->currentTrack = trackNumber;
}

unsigned int Playlist::getCurrentTrackNumber() {
    return this->currentTrack;
}

std::shared_ptr<Track> Playlist::getCurrentTrack() {
    unsigned int trackNr = getCurrentTrackNumber();
    if(tracks.size() == 0 || trackNr > tracks.size()) {
        return nullptr;
    }
    return tracks[trackNr];
}

std::shared_ptr<Track> Playlist::nextTrack() {
    unsigned int trackCount = getTrackCount();
    unsigned int curTrackNumber = getCurrentTrackNumber();
    ++curTrackNumber;
    //Limit to maximum of tracks in list
    if(curTrackNumber >= trackCount) {
        return getCurrentTrack();
    }
    this->currentTrack = curTrackNumber;
    return getCurrentTrack();
}
std::shared_ptr<Track> Playlist::previousTrack() {
    int curTrackNumber = getCurrentTrackNumber();
    //Check if we are already at the beginning (limit to 0)
    if(curTrackNumber > 0) {
        --curTrackNumber;
        //Save value to class member
        currentTrack = curTrackNumber;
    }
    return getCurrentTrack();
}

