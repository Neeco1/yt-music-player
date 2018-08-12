# CMakeLists files in this project can
# refer to the root directory of the project as ${yt-music-player_SOURCE_DIR} and
# to the root binary directory of the project as ${yt-music-player_BIN_DIR}.
cmake_minimum_required (VERSION 3.0)
set (CMAKE_CXX_STANDARD 11)

project (yt-music-player)


# Recurse into the subdirectories
add_subdirectory (lib/)
add_subdirectory (src/)

include_directories (include/)
include_directories (lib/)
include_directories (/usr/local/include/)

add_executable (MusicPlayerService main.cpp)
target_include_directories(MusicPlayerService PUBLIC include/)
target_link_libraries(MusicPlayerService simple-websocket-server WebSocketPlayer)
