#include "MusicPlayer.h"
#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <array>
#include <memory>
#include <regex>
#include "youtube/YoutubeHandler.h"
#include "youtube/YoutubePlaylist.h"
#include "EventBus/HandlerRegistration.hpp"
#include "EventBus/EventBus.hpp"

MusicPlayer::MusicPlayer()
: currentPlaylist(nullptr) {
    regNewPlaylistReady = EventBus::AddHandler<NewPlaylistReadyEvent>(*this);
    Utils::checkDirectory();
    readDataFromJsonFile();
}

MusicPlayer::~MusicPlayer() {
    if(regNewPlaylistReady != nullptr)
    {
        regNewPlaylistReady->removeHandler();
        delete regNewPlaylistReady;
    }
}

bool MusicPlayer::startPlayback() {
    if(!currentPlaylist || (currentPlaylist->isPlaying() && !currentPlaylist->isPaused()))
    {
        return false;
    }
    currentPlaylist->playList();
    return true;
}

bool MusicPlayer::stopPlayback() {
    if(!currentPlaylist) { return false; }
    if(currentPlaylist->isPlaying())
    {
        currentPlaylist->stopPlayback();
        return true;
    }
}

bool MusicPlayer::pausePlayback() {
    if(!currentPlaylist) { return false; }
    if(currentPlaylist->isPlaying())
    {
        currentPlaylist->pausePlayback();
        return true;
    }
    return false;
}

bool MusicPlayer::setVolume(unsigned int volume) {
    if(volume > 100) { volume = 100; }
    
    std::stringstream ssCmd;
    ssCmd << "sudo amixer set PCM " << volume << "%";
    std::string strCommand = ssCmd.str();
    Utils::execCommand(strCommand);
    return true;
}

PlaybackInfo MusicPlayer::getPlaybackInfo() {
    PlaybackInfo info;
    if(!currentPlaylist) { return info; }
    //Build playback info object
    auto track = currentPlaylist->getCurrentTrack();
    if(currentPlaylist->isPlaying()) { info.state = "playing"; }
    else if(currentPlaylist->isPaused()) { info.state = "paused"; }
    info.title = track->getName();
    info.trackId = track->getTrackId();
    info.playbackTime = 0;
    info.duration = 0;
    info.playbackMode = Normal;
    info.thumbUrl = track->getThumbUrl();
    return info;
}

bool MusicPlayer::setPlaybackMode(PlaybackMode mode) {
    if(!currentPlaylist) { return false; }
    currentPlaylist->setPlaybackMode(mode);
    return true;
}

bool MusicPlayer::addPlaylist(const std::shared_ptr<Playlist> playlistPtr) {
    playlists[playlistPtr->getListId()] = playlistPtr;
    return true;
}

std::string MusicPlayer::addPlaylistFromUrl(std::string url, std::string name) {
    if(url.empty()) { return ""; }
    
    //Youtube Playlist URL regex
    std::regex yt_plist_regex("^.*(youtu.be\\/|list=)([^#\\&\\?]*).*");
    
    std::smatch matches;
    //Find out type of playlist
    if(std::regex_match(url, matches, yt_plist_regex))
    {
        //Create new Youtube Handler to fetch playlist info (using youtube-dl)
        std::string listId = matches[2].str();
        YoutubeHandler ytHandler;
        ytHandler.newPlaylist(listId, name);
        return listId;
    }
    std::cout << "No valid url!" << std::endl;
    return "";
    //else if(spotify)
        //...
}

const std::vector<std::shared_ptr<Playlist>> MusicPlayer::getPlaylists() const {
    std::vector<std::shared_ptr<Playlist>> playlistsVec;
    
    for(auto it = playlists.begin(); it != playlists.end(); ++it)
    {
        playlistsVec.push_back(it->second);
    }
    return playlistsVec;
}

const int MusicPlayer::getPlaylistCount() const {
    return playlists.size();
}

bool MusicPlayer::selectPlaylist(const std::string & playlist_id) {
    try
    {
        auto playlistPtr = playlists.at(playlist_id);
        stopPlayback();
        currentPlaylist = playlistPtr;
        return true;
    }
    catch(std::out_of_range ex)
    {
        return false;
    }
}

void MusicPlayer::readDataFromJsonFile() {
    std::string pathname = std::string(getenv("HOME")) + "/websocketPlayer/playlists.json";
    std::cout << "Pathname of json file: " << pathname << std::endl;
    Json::Value jsonData = Utils::readJsonFromFile(pathname);
    //Create playlist objects and add them to the player
    for(Json::Value playlist : jsonData)
    {
        std::string listType = playlist["type"].asString();
        if(listType.compare("YouTube") == 0)
        {
            addPlaylist(std::make_shared<YoutubePlaylist>(playlist));
        }
    }
}

void MusicPlayer::writeDataToJsonFile() {
    std::string pathname = std::string(getenv("HOME")) + "/websocketPlayer/playlists.json";
    Utils::writeJsonToFile(Utils::playlistsToJson(getPlaylists()), pathname);
}

void MusicPlayer::onEvent(NewPlaylistReadyEvent & e) {
    auto playlist = e.getPlaylist();
    std::cout << "Playlist '" << playlist->getName() << "' ready!" << std::endl;
    addPlaylist(playlist);
    writeDataToJsonFile();
}
