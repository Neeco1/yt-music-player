include_directories(${yt-music-player_SOURCE_DIR}/include/)
include_directories(${yt-music-player_SOURCE_DIR}/lib/)
include_directories (/usr/local/include/)

add_library(WebSocketPlayer WebSocketPlayer.cpp)
add_library(MusicPlayer MusicPlayer.cpp Playlist.cpp)
add_library(Utils Utils.cpp)
add_library(MPV_Classes MPV_Controller.cpp MPV_Listener.cpp)
add_library(PlaylistUpdater PlaylistUpdater.cpp)

add_subdirectory(youtube/)

target_link_libraries(MusicPlayer youtube-plugin MPV_Classes PlaylistUpdater)
target_link_libraries(WebSocketPlayer MusicPlayer jsoncpp Utils)

