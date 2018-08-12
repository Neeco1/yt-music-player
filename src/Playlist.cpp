#include "Playlist.h"

Playlist::Playlist() {

}

const std::vector<Track> & Playlist::getAllTracks() const {
    return tracks;
}

void Playlist::addTrack(const Track & track) {
    tracks.push_back(track);
}
