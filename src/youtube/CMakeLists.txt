set (CMAKE_CXX_STANDARD 11)

add_library(youtube-plugin YoutubeHandler.cpp YoutubePlaylist.cpp)
target_link_libraries(youtube-plugin EventBus)
