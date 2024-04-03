#include "server.h"

QScopedPointer<QFile> g_logFile;
QString g_logLvl;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(g_logFile.data());

    switch (type)
    {
    case QtInfoMsg:
        if (g_logLvl.contains('1'))
        {
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
                << "INF "<< context.category << ": "<< msg << endl;
        }
        break;
    case QtDebugMsg:
        if (g_logLvl.contains('2'))
        {
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
                << "DBG "<< context.category << ": "<< msg << endl;
        }
        break;
    case QtWarningMsg:
        if (g_logLvl.contains('3'))
        {
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
                << "WRN "<< context.category << ": "<< msg << endl;
        }
        break;
    case QtCriticalMsg:
        if (g_logLvl.contains('4'))
        {
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
                << "CRT "<< context.category << ": "<< msg << endl;
        }
        break;
    case QtFatalMsg:
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
            << "FTL "<< context.category << ": "<< msg << endl;
        break;
    }

    out.flush();
}

TServer::TServer()
{

}

TServer::~TServer()
{
    amqp_channel_close(m_conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(m_conn);
}

int TServer::connectRabbit()
{
    if (m_socket != NULL)
    {
        amqp_channel_close(m_conn, 1, AMQP_REPLY_SUCCESS);
        amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS);
        amqp_destroy_connection(m_conn);
        m_socket = NULL;
    }

    QSettings settings(QString("settings.ini"),QSettings::IniFormat);

    QString logPath = settings.value("Logging/logPath").toString();
    g_logFile.reset(new QFile(logPath));
    g_logFile.data()->open(QFile::Append | QFile::Text);
    g_logLvl = settings.value("Logging/logLevel").toString();
    qInstallMessageHandler(messageHandler);

    QString strBuf = settings.value("Network/hostname").toString();
    QByteArray byteArray = strBuf.toUtf8();
    const char* hostname = byteArray.constData();
    int port = settings.value("Network/port").toInt();
    QString strBuf2 = settings.value("Network/bindingkey").toString();
    QByteArray byteArray2 = strBuf2.toUtf8();
    char const* bindingkey = byteArray2.constData();
    QString strBuf3 = settings.value("Network/exchange").toString();
    QByteArray byteArray3 = strBuf3.toUtf8();
    char const* exchange = byteArray3.constData();

    int status;
    amqp_bytes_t queuename;

    m_conn = amqp_new_connection();

    m_socket = amqp_tcp_socket_new(m_conn);
    if (m_socket)
    {
        qInfo(logInfo()) << "Create TCP m_socket";
    }
    else
    {
        qCritical(logCritical()) << "Failed when creating TCP m_socket";
        return 1;
    }

    status = amqp_socket_open(m_socket, hostname, port);
    if (status == AMQP_STATUS_OK)
    {
        qInfo(logInfo()) << "Open TCP m_socket";
    }
    else
    {
        qCritical(logCritical()) << "Failed when opening TCP m_socket";
        return 2;
    }

    status = amqp_login(m_conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest").reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Login to the broker";
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when login to the broker: The broker closed the m_socket";
        return 31;
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when login to the broker: The broker returned an exception";
        return 32;
    }

    amqp_channel_open(m_conn, 1);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Open channel";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when open chennel: The broker returned an exception";
        return 41;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when open chennel: An exception occurred within the library";
        return 42;
    }

    amqp_queue_declare_ok_t *r = amqp_queue_declare(m_conn, 1, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Declare queue";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when declare queue: The broker returned an exception";
        return 51;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when declare queue: An exception occurred within the library";
        return 52;
    }

    queuename = amqp_bytes_malloc_dup(r->queue);
    if (queuename.bytes == NULL) {
        qCritical(logCritical()) << "Out of memory while copying queue name";
        return 6;
    }

    amqp_queue_bind(m_conn, 1, queuename, amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey), amqp_empty_table);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Bind queue";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when bind queue: The broker returned an exception";
        return 71;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when bind queue: An exception occurred within the library";
        return 72;
    }

    amqp_basic_consume(m_conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    status = amqp_get_rpc_reply(m_conn).reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Consume successful";
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when consume: The broker returned an exception";
        return 81;
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when consume: An exception occurred within the library";
        return 82;
    }

    return 0;
}

void TServer::consumeAndSendMessage()
{
    std::string serializedMessage;
    TestTask::Messages::Request messageRequest;
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
    messageRequest.ParseFromString(serializedMessage);
    qDebug(logDebug()) <<"Received <<"<<messageRequest.req()<<">> from <<"<<QString::fromStdString(messageRequest.id())<< ">>";

    messageResponse.set_id(messageRequest.id());
    messageResponse.set_res(messageRequest.req()*2);
    messageResponse.SerializeToString(&serializedMessage);

    amqp_bytes_t result;
    result.len = serializedMessage.size();
    result.bytes = (void*)serializedMessage.c_str();

    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;
    props.content_type = amqp_cstring_bytes("number");

    props.delivery_mode = 2;

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
