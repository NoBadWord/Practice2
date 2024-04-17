#ifndef TSERVER_H
#define TSERVER_H

#include "LoggingCategories.h"
#include "proto_file.pb.h"

#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <amqp.h>
#include <amqp_tcp_socket.h>

class TServer
{
private:
    amqp_socket_t *m_socket = NULL;
    amqp_connection_state_t m_conn;

    int createSocket();
    int openSocket(const char* host, int port);

public:
    TServer();
    ~TServer();
    int connectRabbit();
    void disconnectRabbit();
    void consumeAndSendMessage();
    //std::string doubleMessage(std::string &serializedMessage);
};

#endif // TSERVER_H
