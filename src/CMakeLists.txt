include_directories(${yt-music-player_SOURCE_DIR}/include/)
include_directories(${yt-music-player_SOURCE_DIR}/lib/)
include_directories (/usr/local/include/)

add_library(WebSocketPlayer WebSocketPlayer.cpp)
add_library(MusicPlayer MusicPlayer.cpp Playlist.cpp)
add_library(Utils Utils.cpp)
add_library(MPV_Controller MPV_Controller.cpp)

add_subdirectory(youtube/)

target_link_libraries(MusicPlayer youtube-plugin)
target_link_libraries(WebSocketPlayer MusicPlayer jsoncpp Utils)

