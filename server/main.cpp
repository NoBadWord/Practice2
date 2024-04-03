#include <amqp.h>
#include <amqp_tcp_socket.h>
#include "proto_file.pb.h"

#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "LoggingCategories.h"
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
