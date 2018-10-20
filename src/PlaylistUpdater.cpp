#include "PlaylistUpdater.h"
#include "Playlist.h"
#include "MusicPlayer.h"
#include <stdexcept>

PlaylistUpdater::PlaylistUpdater(MusicPlayer * player)
: player(player), threadPool(new ctpl::thread_pool(2))
{}

bool PlaylistUpdater::startUpdate() {
    //Get all playlists from player
    const std::vector<std::shared_ptr<Playlist>> playlists = player->getPlaylists();
    
    //Check one by one if an update is already running
    for(auto it = playlists.begin(); it != playlists.end(); ++it)
    {
        auto playlistShPtr = *it;
        bool doUpdate = false;
        try
        {
            updatesInProgress.at(playlistShPtr->getListId());
            //In list, now check future state
            //TODO
            doUpdate = false;
        }
        catch(std::out_of_range e)
        {
            //Entry not found in map, so do update
            doUpdate = true;
        }
        if(!doUpdate) { continue; }
        
        //Push update task for the playlist on the thread pool
        threadPool->push([playlistShPtr](int id)
        {
            //TODO Implement
        });
    }
}

void PlaylistUpdater::updateList() {

}
