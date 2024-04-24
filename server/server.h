#ifndef TSERVER_H
#define TSERVER_H

#include "LoggingCategories.h"
#include "proto_file.pb.h"
#include "settings.h"

#include <QDebug>

#include <amqp.h>
#include <amqp_tcp_socket.h>

class TServer
{
private:
    amqp_socket_t *m_socket = NULL;
    amqp_connection_state_t m_conn;
    TSettings m_settings;
    int createSocket();
    int openSocket(const char* host, int port);

public:
    TServer();
    ~TServer();
    void setSettings(QString settingsFile);
    int connectRabbit();
    void disconnectRabbit();
    void consumeAndSendMessage();
};

#endif // TSERVER_H
