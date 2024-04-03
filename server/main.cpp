#include "server.h"

int main() {
    TServer server;
    if (server.connectRabbit())
    {
        return 1;
    }
    for(;;)
    {
        server.consumeAndSendMessage();
    }
    return 0;
}
