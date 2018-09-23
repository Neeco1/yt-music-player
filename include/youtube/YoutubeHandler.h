#ifndef YOUTUBE_HANDLER_H__
#define YOUTUBE_HANDLER_H__

#include <vector>
#include <thread>
#include "EventBus/Object.hpp"

class YoutubeHandler : public Object {
private:
    std::vector<std::thread> runningTasks;
    
    /**
     * This is an asynchronous task, which will result in an eventbus event
     * called "PlaylistAdded"
     */
    void fetchPlaylistInfo(std::string playlistId, std::string name);

public:
    void newPlaylist(std::string & playlistId, std::string & name);
    void fetchTrackInfo(std::string & trackId);
    
};

#endif
