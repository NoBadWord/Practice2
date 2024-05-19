#ifndef TSETTINGS_H
#define TSETTINGS_H

#include "LoggingCategories.h"
#include <QString>
#include <QSettings>
#include <QFile>

class TSettings
{
public:
    TSettings();
    QString logPath();
    QString logLvl();
    QString hostname();
    QString bindingkey();
    QString exchange();
    int port();
    void setSettingsFile(QString settingsFile);

private:
    QString m_settingsFile;
    QString m_logPath;
    QString m_logLvl;
    QString m_hostname;
    QString m_bindingkey;
    QString m_exchange;
    int m_port;
    void updateSettings();
};

#endif // TSETTINGS_H
