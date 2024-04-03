#include "server.h"

QScopedPointer<QFile> m_logFile;
QString logLvl;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QTextStream out(m_logFile.data());

    switch (type)
    {
    case QtInfoMsg:
        if (logLvl.contains('1'))
        {
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
                << "INF "<< context.category << ": "<< msg << endl;
        }
        break;
    case QtDebugMsg:
        if (logLvl.contains('2'))
        {
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
                << "DBG "<< context.category << ": "<< msg << endl;
        }
        break;
    case QtWarningMsg:
        if (logLvl.contains('3'))
        {
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
                << "WRN "<< context.category << ": "<< msg << endl;
        }
        break;
    case QtCriticalMsg:
        if (logLvl.contains('4'))
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
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
}

int TServer::connectRabbit()
{
    QSettings settings(QString("settings.ini"),QSettings::IniFormat);

    QString logPath = settings.value("Logging/logPath").toString();
    m_logFile.reset(new QFile(logPath));
    m_logFile.data()->open(QFile::Append | QFile::Text);
    logLvl = settings.value("Logging/logLevel").toString();
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

    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if (socket)
    {
        qInfo(logInfo()) << "Create TCP socket";
    }

    status = amqp_socket_open(socket, hostname, port);
    if (status == AMQP_STATUS_OK)
    {
        qInfo(logInfo()) << "Open TCP socket";
    }

    if (amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,"guest", "guest").reply_type == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Login to the broker";
    }

    amqp_channel_open(conn, 1);
    if (amqp_get_rpc_reply(conn).reply_type == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Open channel";
    }

    amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, 1, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
    if (amqp_get_rpc_reply(conn).reply_type == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Declare queue";
    }

    queuename = amqp_bytes_malloc_dup(r->queue);

    if (queuename.bytes == NULL) {
        qCritical(logCritical()) << "Out of memory while copying queue name";
        return 1;
    }


    amqp_queue_bind(conn, 1, queuename, amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey), amqp_empty_table);
    if (amqp_get_rpc_reply(conn).reply_type == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Bind queue";
    }

    amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    if (amqp_get_rpc_reply(conn).reply_type == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Consume";
    }
    return 0;
}

void TServer::consumeAndSendMessage()
{
    std::string serialized_message;
    TestTask::Messages::Request messageRequest;
    TestTask::Messages::Response messageResponse;
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;

    amqp_maybe_release_buffers(conn);

    res = amqp_consume_message(conn, &envelope, NULL, 0);

    serialized_message = std::string((char*)envelope.message.properties.content_type.bytes,envelope.message.properties.content_type.len);
    messageRequest.ParseFromString(serialized_message);
    qDebug(logDebug()) <<"Received <<"<<messageRequest.req()<<">> from <<"<<QString::fromStdString(messageRequest.id())<< ">>";

    messageResponse.set_id(messageRequest.id());
    messageResponse.set_res(messageRequest.req()*2);
    messageResponse.SerializeToString(&serialized_message);

    amqp_bytes_t result;
    result.len = serialized_message.size();
    result.bytes = (void*)serialized_message.c_str();

    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;
    props.content_type = result;

    props.delivery_mode = 2;

    amqp_basic_publish(conn,1,amqp_empty_bytes,amqp_cstring_bytes((char *)envelope.message.properties.reply_to.bytes),0,0,&props, result),

    qDebug(logDebug()) <<"Sent <<"<<messageResponse.res()<<">> to <<"<<QString::fromStdString(messageResponse.id())<< ">>";

    amqp_destroy_envelope(&envelope);
}
