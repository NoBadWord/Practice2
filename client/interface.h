#ifndef TINTERFACE_H
#define TINTERFACE_H

#include "settingsmenu.h"
#include "proto_file.pb.h"
#include "LoggingCategories.h"

#include <QMainWindow>
#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <amqp.h>
#include <amqp_tcp_socket.h>

QT_BEGIN_NAMESPACE
namespace Ui { class TInterface; }
QT_END_NAMESPACE

class TInterface : public QMainWindow
{
    Q_OBJECT

public:
    TInterface(QWidget *parent = nullptr);
    ~TInterface();
    int connectRabbit();

private slots:
    void on_SendNumberBtn_clicked();
    void on_settingsMenuBtn_triggered();

private:
    Ui::TInterface *ui;
    TSettingsMenu m_settingsMenu;
    amqp_socket_t *m_socket = NULL;
    amqp_connection_state_t m_conn;
    amqp_bytes_t m_reply_to_queue;

    void sendMessage();
    void consumeMessage();
};
#endif // TINTERFACE_H
