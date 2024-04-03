#ifndef TINTERFACE_H
#define TINTERFACE_H

#include <QMainWindow>
#include <settingsmenu.h>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include "proto_file.pb.h"

#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "LoggingCategories.h"

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
    TSettingsMenu settingsMenu;
    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;
    amqp_bytes_t reply_to_queue;

    void sendMessage();
    void consumeMessage();
};
#endif // TINTERFACE_H
