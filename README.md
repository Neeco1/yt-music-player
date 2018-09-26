# yt-music-player

A music player which can be controlled using a web interface based on websockets. This is still a work in progress!
I started developing this project because I needed a simple and easy-to-use music player for my Raspberry Pi.

It works, but might still have a lot of bugs and the webclient is a quick-and-dirty one.

Currently the player supports only youtube tracks and playlists. Feel free to add support for more services!

### Youtube Plugin:
- Uses [`mpv` player](https://mpv.io) in socket mode (using `--input-ipc-server=/tmp/mpvsocket`)
- Uses `youtube-dl` to fetch playlist and video information

## Setup
- First: ``sudo apt-get update && sudo apt-get upgrade``
- Install necessary packages: ``sudo apt-get install -y python python3-pip mpv socat libboost-all-dev``
- Install youtube-dl: ``pip install youtube-dl``
- Go to directory of the repository: ``git clone https://github.com/Neeco1/yt-music-player`` and ``cd yt-music-player``
- ``mkdir build && cd build``
- ``cmake ..``
- ``make``
- ``./MusicPlayerService`` (or run it in a [`screen`](https://wiki.ubuntuusers.de/Screen/) to put it in background)

## Developers

### Websocket API for Communication

```
{
    'cmd' : '<command>',
    'data' : {}
}
```

Possible commands:
* stop
* play
* set_volume
* get_playlists
* add_playlist

For details, see the file [WebSocketPlayer.cpp](https://github.com/Neeco1/yt-music-player/blob/master/src/WebSocketPlayer.cpp)
