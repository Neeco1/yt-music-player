#include "WebSocketPlayer.h"

int main(int argc, char * argv[]) {

    WebSocketPlayer player(8080);
    std::thread playerThread = player.startThread();
    playerThread.join();
}
