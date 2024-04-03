#ifndef TSERVER_H
#define TSERVER_H

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include "proto_file.pb.h"

#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "LoggingCategories.h"

class TServer
{
private:
    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;
    //amqp_envelope_t envelope;
public:
    TServer();
    ~TServer();
    int connectRabbit();
    void consumeAndSendMessage();
};

#endif // TSERVER_H
