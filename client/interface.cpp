#include "interface.h"
#include "ui_interface.h"

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

TInterface::TInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TInterface)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Удвоение через брокер"));
    //connectRabbit();

}

TInterface::~TInterface()
{
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
    delete ui;
}

int TInterface::connectRabbit()
{
    if (socket != NULL)
    {
        amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
        amqp_destroy_connection(conn);
        socket = NULL;
    }

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

    int status;

    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if (socket)
    {
        qInfo(logInfo()) << "Create TCP socket";
    }
    else
    {
        qCritical(logCritical()) << "Failed when creating TCP socket";
        return 1;
    }

    status = amqp_socket_open(socket, hostname, port);
    if (status == AMQP_STATUS_OK)
    {
        qInfo(logInfo()) << "Open TCP socket";
    }
    else
    {
        qCritical(logCritical()) << "Failed when opening TCP socket";
        return 2;
    }

    status = amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,"guest", "guest").reply_type;
    if (status == AMQP_RESPONSE_NORMAL)
    {
        qInfo(logInfo()) << "Login to the broker";
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when login to the broker: The broker closed the socket";
        return 31;
    }
    else if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when login to the broker: The broker returned an exception";
        return 32;
    }

    amqp_channel_open(conn, 1);
    status = amqp_get_rpc_reply(conn).reply_type;
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

    amqp_queue_declare_ok_t *r = amqp_queue_declare( conn, 1, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
    status = amqp_get_rpc_reply(conn).reply_type;
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

    reply_to_queue = amqp_bytes_malloc_dup(r->queue);
    if (reply_to_queue.bytes == NULL)
    {
      qCritical(logCritical()) << "Out of memory while copying queue name";
      return 6;
    }
    return 0;
}

void TInterface::sendMessage()
{
    QSettings settings(QString("settings.ini"),QSettings::IniFormat);

    QString logPath = settings.value("Logging/logPath").toString();
    m_logFile.reset(new QFile(logPath));
    m_logFile.data()->open(QFile::Append | QFile::Text);
    QString strBuf = settings.value("Network/routingkey").toString();
    QByteArray byteArray = strBuf.toUtf8();
    char const* routingkey = byteArray.constData();
    QString strBuf2 = settings.value("Network/exchange").toString();
    QByteArray byteArray2 = strBuf2.toUtf8();
    char const* exchange = byteArray2.constData();
    QString strBuf3 = settings.value("User/id").toString();
    std::string userID = strBuf3.toStdString();

    TestTask::Messages::Request messageRequest;
    std::string serialized_message;
    int messagebody;
    messagebody = ui->numberLineEdit->text().toInt();
    messageRequest.set_id(userID);
    messageRequest.set_req(messagebody);
    messageRequest.SerializeToString(&serialized_message);

    amqp_bytes_t result;
    result.len = serialized_message.size();
    result.bytes = (void*)serialized_message.c_str();

    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_REPLY_TO_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;

    props.content_type = result;

    props.delivery_mode = 2;
    props.reply_to = amqp_bytes_malloc_dup(reply_to_queue);
    if (props.reply_to.bytes == NULL)
    {
        qCritical(logCritical()) << "Out of memory while copying queue name";
    }

    props.correlation_id = amqp_cstring_bytes(routingkey);

    if(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange), amqp_cstring_bytes(routingkey), 0, 0, &props, result) != AMQP_STATUS_OK)
    {
        qWarning(logWarning()) << "Failed when publish message";
    }
    else
    {
        qDebug(logDebug()) << "Number" << messageRequest.req() << "is publish";
    }
    amqp_bytes_free(props.reply_to);

    amqp_basic_consume(conn, 1, reply_to_queue, amqp_empty_bytes, 0, 1, 0,
                       amqp_empty_table);
    int status = amqp_get_rpc_reply(conn).reply_type;
    if (status == AMQP_RESPONSE_SERVER_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when consume: The broker returned an exception";
    }
    else if (status == AMQP_RESPONSE_LIBRARY_EXCEPTION)
    {
        qCritical(logCritical()) << "Failed when consume: An exception occurred within the library";
    }
}

void TInterface::consumeMessage()
{
    TestTask::Messages::Response messageResponse;
    std::string serialized_message;
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_REPLY_TO_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;
    timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    amqp_maybe_release_buffers(conn);
    res = amqp_consume_message(conn, &envelope, &timeout, 0);
    if (res.reply_type == AMQP_RESPONSE_NORMAL)
    {
        serialized_message = std::string((char*)envelope.message.properties.content_type.bytes,envelope.message.properties.content_type.len);
        messageResponse.ParseFromString(serialized_message);
        ui->outputLabel->setText(QString::number(messageResponse.res()));
        qDebug(logDebug()) << "Number" << messageResponse.res() << "is consume";
    }
    else
    {
        ui->outputLabel->setText(QString("SERVER ERROR"));
        qCritical(logCritical()) << "SERVER ERROR WHEN CONSUME MESSAGE";
    }

    amqp_destroy_envelope(&envelope);
}

void TInterface::on_SendNumberBtn_clicked()
{
    sendMessage();
    consumeMessage();
}

void TInterface::on_settingsMenuBtn_triggered()
{
    settingsMenu.show();
}

