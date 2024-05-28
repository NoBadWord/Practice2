#ifndef TSERVER_H
#define TSERVER_H

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
    int loginRabbit();
    int openChannel();
    amqp_queue_declare_ok_t *declareQueue();
    int bindQueue(amqp_bytes_t &queuename, const char* exchange, const char* bindingkey);
    int basicConsume(amqp_bytes_t &queuename); 

public:
    TServer(const TSettings &settings);
    ~TServer();
    void setSettings(const TSettings &settings);
    int connectRabbit();
    void disconnectRabbit();
    void consumeAndSendMessage();
    std::string doubleMessage(std::string &serializedMessage);
};

#endif // TSERVER_H
