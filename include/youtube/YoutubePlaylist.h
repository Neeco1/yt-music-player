#ifndef YOUTUBE_PLAYLIST_H__
#define YOUTUBE_PLAYLIST_H__

#include "Playlist.h"

class YoutubePlaylist : public Playlist {
public:
    void playTrack(int trackIndex);
    void stopPlayback();
};

#endif
