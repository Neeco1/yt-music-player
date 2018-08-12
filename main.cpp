#include "WebSocketPlayer.h"

int main() {
    WebSocketPlayer player(8080);
    std::thread playerThread = player.start();
    playerThread.join();
}
