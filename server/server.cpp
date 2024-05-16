#include "server.h"

extern QScopedPointer<QFile> g_logFile;
extern QString g_logLvl;

TServer::TServer(QString settingsFile)
{
    m_settings.setSettingsFile(settingsFile);
    QString logPath = m_settings.logPath();
    g_logFile.reset(new QFile(logPath));
    g_logFile.data()->open(QFile::Append | QFile::Text);
    g_logLvl = m_settings.logLvl();
    qInstallMessageHandler(messageHandler);
}

TServer::~TServer()
{
    amqp_channel_close(m_conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_conn);
}

void TServer::disconnectRabbit()
{
    amqp_channel_close(m_conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_conn);
    m_socket = NULL;
}

void TServer::setSettings(QString settingsFile)
{
    m_settings.setSettingsFile(settingsFile);
}

int TServer::createSocket()
{
    m_socket = amqp_tcp_socket_new(m_conn);
    if (m_socket)
    {
        qInfo(logInfo()) << "Create TCP socket";
    }
    else
    {
        qCritical(logCritical()) << "Failed when creating TCP socket";
        return 1;
    }
    return 0;
}

int TServer::openSocket(const char* host, int port)
{
    int status;
    status = amqp_socket_open(m_socket, host, port);
    if (status == AMQP_STATUS_OK)
    {
        qInfo(logInfo()) << "Open TCP socket";
    }
    else
    {
        qCritical(logCritical()) << "Failed when opening TCP socket";
        return 1;
    }
    return 0;
}

int TServer::loginRabbit()
{
    int status;
    status = amqp_login(m_conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest").reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Login to the broker";
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when login to the broker: The broker closed the socket";
        return 1;
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when login to the broker: The broker returned an exception";
        return 2;
    }
    return 0;
}

int TServer::openChannel()
{
    int status;
    amqp_channel_open(m_conn, 1);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Open channel";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when open chennel: The broker returned an exception";
        return 1;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when open chennel: An exception occurred within the library";
        return 2;
    }
    return 0;
}

amqp_queue_declare_ok_t *TServer::declareQueue()
{
    int status;
    amqp_queue_declare_ok_t *r = amqp_queue_declare(m_conn, 1, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Declare queue";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when declare queue: The broker returned an exception";
        return NULL;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when declare queue: An exception occurred within the library";
        return NULL;
    }
    return r;
}

int TServer::bindQueue(amqp_bytes_t &queuename, const char* exchange, const char* bindingkey)
{
    int status;
    amqp_queue_bind(m_conn, 1, queuename, amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey), amqp_empty_table);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Bind queue";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when bind queue: The broker returned an exception";
        return 1;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when bind queue: An exception occurred within the library";
        return 2;
    }
    return 0;
}

int TServer::basicConsume(amqp_bytes_t &queuename)
{
    int status;
    amqp_basic_consume(m_conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Consume successful";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when consume: The broker returned an exception";
        return 1;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when consume: An exception occurred within the library";
        return 2;
    }
    return 0;
}

int TServer::connectRabbit()
{
    if (m_socket != NULL)
    {
        disconnectRabbit();
    }

    QString strBuf = m_settings.hostname();
    QByteArray byteArray = strBuf.toUtf8();
    const char* hostname = byteArray.constData();
    int port = m_settings.port();
    QString strBuf2 = m_settings.bindingkey();
    QByteArray byteArray2 = strBuf2.toUtf8();
    char const* bindingkey = byteArray2.constData();
    QString strBuf3 = m_settings.exchange();
    QByteArray byteArray3 = strBuf3.toUtf8();
    char const* exchange = byteArray3.constData();

    m_conn = amqp_new_connection();

    if (createSocket())
        return 1;

    if (openSocket(hostname, port))
        return 2;

    if (loginRabbit())
        return 3;

    if (openChannel())
        return 4;

     amqp_queue_declare_ok_t *r = declareQueue();
     if (r == NULL)
         return 5;

     amqp_bytes_t queuename;
     queuename = amqp_bytes_malloc_dup(r->queue);
     if (queuename.bytes == NULL) {
         qCritical(logCritical()) << "Out of memory while copying queue name";
         return 6;
     }

    if (bindQueue(queuename, exchange, bindingkey))
        return 7;

    if (basicConsume(queuename))
        return 8;

    return 0;
}

std::string TServer::doubleMessage(std::string &serializedMessage)
{
    std::string serializedMessageToSend;
    TestTask::Messages::Request messageRequest;
    TestTask::Messages::Response messageResponse;

    messageRequest.ParseFromString(serializedMessage);
    qDebug(logDebug()) <<"Received <<"<<messageRequest.req()<<">> from <<"<<QString::fromStdString(messageRequest.id())<< ">>";

    messageResponse.set_id(messageRequest.id());
    messageResponse.set_res(messageRequest.req()*2);
    messageResponse.SerializeToString(&serializedMessageToSend);
    return serializedMessageToSend;
}

void TServer::consumeAndSendMessage()
{
    std::string serializedMessage;
    std::string serializedMessageToSend;
    TestTask::Messages::Response messageResponse;
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;

    amqp_maybe_release_buffers(m_conn);

    res = amqp_consume_message(m_conn, &envelope, NULL, 0);
    if (res.reply_type != AMQP_RESPONSE_NORMAL)
    {
        qCritical(logCritical()) << "Failed when consume message";
    }

    serializedMessage = std::string((char*)envelope.message.body.bytes, envelope.message.body.len);

    serializedMessageToSend = doubleMessage(serializedMessage);
    messageResponse.ParseFromString(serializedMessageToSend);

    amqp_bytes_t result;
    result.len = serializedMessageToSend.size();
    result.bytes = (void*)serializedMessageToSend.c_str();

    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;
    props.content_type = amqp_cstring_bytes("number");

    props.delivery_mode = 2;
    props.correlation_id = amqp_cstring_bytes(messageResponse.id().c_str());

    if (amqp_basic_publish(m_conn,1,amqp_empty_bytes,amqp_cstring_bytes((char *)envelope.message.properties.reply_to.bytes),0,0,&props, result) != AMQP_STATUS_OK)
    {
        qCritical(logCritical()) << "Failed when publish message";
    }
    else
    {
        qDebug(logDebug()) <<"Sent <<"<<messageResponse.res()<<">> to <<"<<QString::fromStdString(messageResponse.id())<< ">>";
    }

    amqp_destroy_envelope(&envelope);
}

