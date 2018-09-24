# yt-music-player

A music player using websockets. Currently supports youtube tracks and playlists. Feel free
to add support for more services.

### Youtube Plugin:
- Uses [https://mpv.io](`mpv` player) in socket mode (using `--input-ipc-server=/tmp/mpvsocket`)
- Uses `youtube-dl` to fetch playlist and video information

## Setup
- First: ``sudo apt-get update && sudo apt-get upgrade``
- Install necessary packages: ``sudo apt-get install -y python python3-pip mpv socat``
- Install youtube-dl: ``pip install youtube-dl``
- Go to directory of the repository: ``git clone https://github.com/Neeco1/yt-music-player`` and ``cd yt-music-player``
- ``mkdir build && cd build``
- ``cmake ..``
- ``make``
- ``./MusicPlayerService

## Websocket API for Communication

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
