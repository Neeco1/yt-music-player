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

Playlist::Playlist() : currentTrack(0), nowPlaying(false), nowPaused(false)
{
    //Initially set timestamped id
    std::stringstream ss;
    ss << "list_" << Utils::getTimeString();
    setListId(ss.str());
    setName("New List");
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

void Playlist::setPlaybackMode(PlaybackMode playbackMode) {
    this->playbackMode = playbackMode;
    if(playbackMode == Shuffle)
    {
        shuffledOrder = tracks;
        std::random_shuffle ( shuffledOrder.begin(), shuffledOrder.end() );
    }
}

const std::vector<std::shared_ptr<Track>> & Playlist::getAllTracks() const {
    return tracks;
}

const unsigned int Playlist::getTrackCount() const {
    return tracks.size();
}

bool Playlist::isPlaying() const {
    return this->nowPlaying;
}
bool Playlist::isPaused() const {
    return nowPaused;
}

unsigned int Playlist::getCurrentTrackNumber() {
    return this->currentTrack;
}

std::shared_ptr<Track> Playlist::getCurrentTrack() {
    return tracks[getCurrentTrackNumber()];
}

bool Playlist::startPlaying() {
    playTrack(currentTrack);
}

std::shared_ptr<Track> Playlist::nextTrack() {
    unsigned int trackCount = getTrackCount();
    unsigned int curTrackNumber = getCurrentTrackNumber();
    ++curTrackNumber;
    if(curTrackNumber >= trackCount) {
        return getCurrentTrack();
    }
    this->currentTrack = curTrackNumber;
    playTrack(curTrackNumber);
    return getCurrentTrack();
}
std::shared_ptr<Track> Playlist::previousTrack() {
    int curTrackNumber = getCurrentTrackNumber();
    //Check if we are already at the beginning
    if(curTrackNumber > 0) {
        --curTrackNumber;
        //Save value to class member
        currentTrack = curTrackNumber;
        playTrack(currentTrack);
    }
    return getCurrentTrack();
}

