#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include "LoggingCategories.h"

#include <QWidget>
#include <QSettings>
#include <QCloseEvent>

namespace Ui {
class TSettingsMenu;
}

class TSettingsMenu : public QWidget
{
    Q_OBJECT

public:
    explicit TSettingsMenu(QWidget *parent = nullptr);
    ~TSettingsMenu();
    QString logPath();
    QString logLvl();
    QString hostname();
    QString routingkey();
    QString exchange();
    int port();
    QString userID();
    void setSettingsFile(QString settingsFile);

private slots:
    void on_saveBtn_clicked();

private:
    Ui::TSettingsMenu *ui;
    QString m_settingsFile;
    QString m_logPath;
    QString m_logLvl;
    QString m_hostname;
    QString m_routingkey;
    QString m_exchange;
    int m_port;
    QString m_userID;
    void updateSettings();

signals:
    void saveSettingsSignal();
};

#endif // SETTINGSMENU_H
