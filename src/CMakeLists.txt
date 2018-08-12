include_directories(${yt-music-player_SOURCE_DIR}/include/)
include_directories(${yt-music-player_SOURCE_DIR}/lib/)
include_directories (/usr/local/include/)

add_library(WebSocketPlayer WebSocketPlayer.cpp)
add_library(MusicPlayer MusicPlayer.cpp)
target_link_libraries(WebSocketPlayer MusicPlayer)
