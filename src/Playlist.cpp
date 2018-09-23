#include "Playlist.h"

#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>
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

const int Playlist::getTrackCount() const {
    return tracks.size();
}

bool Playlist::isPlaying() const {
    return this->nowPlaying;
}
bool Playlist::isPaused() const {
    return nowPaused;
}

int Playlist::getCurrentTrackNumber() {
    return this->currentTrack;
}

std::shared_ptr<Track> Playlist::getCurrentTrack() {
    return tracks[getCurrentTrackNumber()];
}

bool Playlist::startPlaying() {
    playTrack(currentTrack);
}

