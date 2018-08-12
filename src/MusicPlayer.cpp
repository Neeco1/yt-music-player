#include "MusicPlayer.h"

MusicPlayer::MusicPlayer() {}

bool MusicPlayer::addPlaylist(const Playlist & playlist) {
    playlists.push_back(playlist);
    return true;
}

bool addPlaylistFromUrl(std::string url) {
    if(url.empty()) { return false; }
    
    //TODO Contact youtube using youtube dl in another thread
}

const std::vector<Playlist> & MusicPlayer::getPlaylists() const {
    return playlists;
}

const int MusicPlayer::getPlaylistCount() const {
    return playlists.size();
}
