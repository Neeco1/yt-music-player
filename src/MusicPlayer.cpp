#include "PlaylistUpdater.h"
#include "MusicPlayer.h"
#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <array>
#include <memory>
#include <chrono>
#include "BasicPlaylist.h"
#include "youtube/YoutubeHandler.h"
#include "youtube/YoutubePlaylist.h"
#include "EventBus/HandlerRegistration.hpp"
#include "EventBus/EventBus.hpp"
#include "MPV_Controller.h"
#include "MPV_Listener.h"
#include "URLParser.h"

MusicPlayer::MusicPlayer()
: currentPlaylist(nullptr),
  stopPressed(false), prevPressed(false), nextPressed(false),
  newPlaylistSelected(false),
  currentPlaybackTime(0), playbackState(Stopped),
  playlistUpdater(new PlaylistUpdater(this))
{
    
    Utils::checkDirectory();
    readDataFromJsonFile();
    
    handlerRegs.push_back(EventBus::AddHandler<NewPlaylistReadyEvent>(*this));
    handlerRegs.push_back(EventBus::AddHandler<FileEndPlayingEvent>(*this));
    handlerRegs.push_back(EventBus::AddHandler<FileStartPlayingEvent>(*this));
    handlerRegs.push_back(EventBus::AddHandler<PlaybackTimeUpdatedEvent>(*this));

    defaultPlaylist = std::make_shared<BasicPlaylist>();
    currentPlaylist = defaultPlaylist;
    
    MPV_Controller::startMPVIdle();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    MPV_Listener::startMPVListener();
}

MusicPlayer::~MusicPlayer() {
    for(auto handlerReg : handlerRegs)
    {
        if(handlerReg != nullptr)
        {
            handlerReg->removeHandler();
            delete handlerReg;
        }
    }
}

bool MusicPlayer::startPlayback() {
    //Don't do anything if no playlist is selected or if it is already playing
    if(!currentPlaylist || (playbackState == Playing))
    {
        return false;
    }
    
    //Resume if paused
    if(playbackState == Paused)
    {
        MPV_Controller::sendCommandFromMap("cmdUnpausePlayback");
        currentPlaylist->setPlaying(true);
        playbackState = Playing;
        return true;
    }
    
    //Start playback if was stopped previously
    if(playbackState == Stopped)
    {
        auto currentTrack = currentPlaylist->getCurrentTrack();
        if(currentTrack == nullptr) {
            return false;
        }
        currentPlaylist->setPlaying(true);
        MPV_Controller::playMedia(currentTrack->getUrl());
        playbackState = Playing;
        return true;
    }
    
    return false;
}
//public
bool MusicPlayer::stopPlayback() {
    stopPressed = true;
    return stop();
}
//private
bool MusicPlayer::stop() {
    if(!currentPlaylist) { return false; }
    
    //First press: Stop actual playback
    if(playbackState == Playing || playbackState == Paused)
    {
        MPV_Controller::sendCommandFromMap("cmdStopPlayback");
        playbackState = Stopped;
        currentPlaylist->setPlaying(false);
        return true;
    }
    
    //Second and further presses of stop: Reset current track
    if(playbackState == Stopped)
    {
        currentPlaylist->setCurrentTrackNumber(0);
        currentPlaylist->setPlaying(false);
        playbackState = Stopped;
        return true;
    }
    return false;
}

bool MusicPlayer::pausePlayback() {
    if(!currentPlaylist) { return false; }
    
    //Only do something if we are currently playing
    if(playbackState == Playing)
    {
        MPV_Controller::sendCommandFromMap("cmdPausePlayback");
        playbackState = Paused;
        return true;
    }
    return false;
}
//public
bool MusicPlayer::playNext() {
    nextPressed = true;
    return nextTrack();
}
//private
bool MusicPlayer::nextTrack() {
    if(!currentPlaylist) { return false; }
    
    //Only go to next if we are currently playing
    //if(playbackState == Playing)
    //{
    auto nextTrack = currentPlaylist->nextTrack();
    if(!nextTrack) { return false; }
    MPV_Controller::playMedia(nextTrack->getUrl());
    return true;
    //}
    //return false;
}
//public
bool MusicPlayer::playPrevious() {
    prevPressed = true;
    return previousTrack();
}
//private
bool MusicPlayer::previousTrack() {
    if(!currentPlaylist) { return false; }
    
    //Only go to previous if we are currently playing
    //if(playbackState == Playing)
    //{
    auto prevTrack = currentPlaylist->previousTrack();
    if(!prevTrack) { return false; }
    MPV_Controller::playMedia(prevTrack->getUrl());
    return true;
    //}
    //return false;
}

bool MusicPlayer::playTrackFromCurrentListWithIndex(unsigned int index) {
    if(!currentPlaylist) { return false; }
    
    currentPlaylist->setCurrentTrackNumber(index);
    return startPlayback();
}

bool MusicPlayer::setVolume(unsigned int volume) {
    if(volume > 100) { volume = 100; }
    
    std::stringstream ssCmd;
    ssCmd << "sudo amixer set PCM " << volume << "%";
    std::string strCommand = ssCmd.str();
    Utils::execCommand(strCommand);
    return true;
}

bool MusicPlayer::playMediaFromUrl(std::string & url) {
    stopPlayback();
    stopPlayback();
    auto newTrack = std::make_shared<Track>(url);
    newTrack->setName(url);
    newTrack->setUrl(url);
    defaultPlaylist->addTrack(newTrack);
    currentPlaylist = defaultPlaylist;
    currentPlaylist->setPlaying(true);
    MPV_Controller::playMedia(url);
    playbackState = Playing;
    return true;
}

PlaybackInfo MusicPlayer::getPlaybackInfo() const {
    PlaybackInfo info;
    if(!currentPlaylist) { return info; }
    
    //Build playback info object
    if(playbackState == Paused) { info.state = "paused"; }
    else if(playbackState == Playing) { info.state = "playing"; }
    else if(playbackState == Stopped) { info.state = "stopped"; }

    auto track = currentPlaylist->getCurrentTrack();
    if(track != nullptr) 
    {
        info.title = track->getName();
        info.trackId = track->getTrackId();
        info.playbackTime = currentPlaybackTime;
        info.duration = track->getDurationSeconds();
        info.playbackMode = Normal;
        info.thumbUrl = track->getThumbUrl();
    }    
    return info;
}

PlaybackState MusicPlayer::getPlaybackState() const {
    return playbackState;
}

bool MusicPlayer::setPlaybackMode(PlaybackMode mode) {
    if(!currentPlaylist) { return false; }
    this->playbackMode = playbackMode;
    
    //TODO handle Shuffle
    /*if(playbackMode == Shuffle)
    {
        shuffledOrder = tracks;
        std::random_shuffle ( shuffledOrder.begin(), shuffledOrder.end() );
    }*/
    
    return true;
}

bool MusicPlayer::addPlaylist(const std::shared_ptr<Playlist> playlistPtr) {
    playlists[playlistPtr->getListId()] = playlistPtr;
    return true;
}

std::string MusicPlayer::addPlaylistFromUrl(std::string url, std::string name) {
    if(url.empty()) { return ""; }
    
    std::string listId;
    if(URLParser::parseURL_YoutubePlaylist(url, listId))
    {
        //Create new Youtube Handler to fetch playlist info (using youtube-dl)
        YoutubeHandler ytHandler;
        ytHandler.newPlaylist(listId, name);
        return listId;
    }
    //else if(spotify)
    //...
    return "";
}

bool MusicPlayer::updatePlaylists() {
    playlistUpdater->startUpdate();
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
        //Stop playback if playing
        if(playbackState == Playing)
        {
            newPlaylistSelected = true;
            stop();
        }
        auto playlistPtr = playlists.at(playlist_id);
        currentPlaylist = playlistPtr;
        currentPlaylist->setPlaying(true);
        return true;
    }
    catch(std::out_of_range ex)
    {
        return false;
    }
}

bool MusicPlayer::setPlaybackTime(std::string time) {
    return MPV_Controller::setProperty("playback-time", time);
}

void MusicPlayer::readDataFromJsonFile() {
    std::string pathname = std::string(getenv("HOME")) + "/websocketPlayer/playlists.json";
    //std::cout << "Pathname of json file: " << pathname << std::endl;
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
    addPlaylist(playlist);
    writeDataToJsonFile();
}

void MusicPlayer::onEvent(FileStartPlayingEvent & e) {
    currentPlaybackTime = 0;
    stopPressed = false;
    prevPressed = false;
    nextPressed = false;
}

void MusicPlayer::onEvent(FileEndPlayingEvent & e) {
    currentPlaybackTime = 0;
    if(!currentPlaylist) { return; }
    //Prevent playback of next track from this event immediately after user
    //pressed stop, previous or next
    if(stopPressed || prevPressed || nextPressed || newPlaylistSelected)
    {
        stopPressed = false;
        prevPressed = false;
        nextPressed = false;
        newPlaylistSelected = false;
        return;
    }
    nextTrack();
}

void MusicPlayer::onEvent(PlaybackTimeUpdatedEvent & e) {
    currentPlaybackTime = e.getPlaybackTime();
}
