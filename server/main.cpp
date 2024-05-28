#include "server.h"

int main() {
    setLog("settings.ini");
    TSettings settings;
    settings = readSettings("settings.ini");
    TServer server(settings);

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
