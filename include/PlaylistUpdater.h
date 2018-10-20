#ifndef PLAYLIST_UPDATER_H__
#define PLAYLIST_UPDATER_H__

#include <thread_pool/ctpl_stl.h>
#include <future>
#include <string>
#include <map>

typedef struct UpdateEntry {
    std::string playlist_id;
    std::future<void> future;
} UpdateEntry;


class MusicPlayer;
class PlaylistUpdater {
private:
    MusicPlayer * player;
    ctpl::thread_pool * threadPool; //TODO Find a way to use ctpl without pointer here
    
    //Map for entries of playlists currently being updated
    std::map<std::string, UpdateEntry> updatesInProgress;

public:
    PlaylistUpdater(MusicPlayer * player);
    ~PlaylistUpdater() {};

    bool startUpdate();
    
    static void updateList();

};

#endif
