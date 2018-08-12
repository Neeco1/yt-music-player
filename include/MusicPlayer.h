#ifndef MUSICPLAYER_H__
#define MUSICPLAYER_H__

#include "Playlist.h"
#include "Track.h"

class MusicPlayer {
private:
    std::vector<Playlist> playlists;
    

public:
    MusicPlayer();
    
    bool addPlaylist(const Playlist & playlist);
    bool addPlaylistFromUrl(std::string url);
    const std::vector<Playlist> & getPlaylists() const;
    const int getPlaylistCount() const;
    
};


#endif
