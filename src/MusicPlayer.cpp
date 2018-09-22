#include "MusicPlayer.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include <memory>
#include <regex>
#include "youtube/YoutubeHandler.h"
#include "EventBus/HandlerRegistration.hpp"
#include "EventBus/EventBus.hpp"

MusicPlayer::MusicPlayer() {
    regNewPlaylistReady = EventBus::AddHandler<NewPlaylistReadyEvent>(*this);
}

MusicPlayer::~MusicPlayer() {
    if(regNewPlaylistReady != nullptr)
    {
        regNewPlaylistReady->removeHandler();
        delete regNewPlaylistReady;
    }
}

bool MusicPlayer::startPlaying() {
    if(currentPlaylist->isPlaying())
    {
        return false;
    }
    currentPlaylist->startPlaying();
}

bool MusicPlayer::stopPlaying() {
    if(currentPlaylist->isPlaying())
    {
    
    }
}

bool MusicPlayer::setVolume(int volume) {
    if(volume < 0 || volume > 100) { return false; }
    
    std::stringstream cmd;
    cmd << "sudo amixer set PCM " << volume << "%";
    std::string strCommand = cmd.str();
    Utils::execCommand(strCommand);
    return true;
}

bool MusicPlayer::addPlaylist(const std::shared_ptr<Playlist> playlist) {
    playlists.push_back(playlist);
    return true;
}

bool MusicPlayer::addPlaylistFromUrl(std::string & url) {
    if(url.empty()) { return false; }
    
    //Youtube Playlist URL regex
    std::regex yt_plist_regex("^.*(youtu.be\\/|list=)([^#\\&\\?]*).*");
    
    std::smatch matches;
    //Find out type of playlist
    if(std::regex_match(url, matches, yt_plist_regex))
    {
        //Create new Youtube Handler to fetch playlist info (using youtube-dl)
        std::string listId = matches[2].str();
        YoutubeHandler ytHandler;
        ytHandler.newPlaylist(listId);
        return true;
    }
    std::cout << "No valid url!" << std::endl;
    return false;
    //else if(spotify)
        //...
}

const std::vector<std::shared_ptr<Playlist>> & MusicPlayer::getPlaylists() const {
    return playlists;
}

const int MusicPlayer::getPlaylistCount() const {
    return playlists.size();
}


void MusicPlayer::onEvent(NewPlaylistReadyEvent & e) {
    // Print out the name of the player and the chat message
    auto playlist = e.getPlaylist()
    std::cout << "Playlist '" << playlist->getName() << "' ready!" << std::endl;
    addPlaylist(playlist);
}
