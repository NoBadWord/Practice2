#include "settings.h"

TSettings readSettings(QString settingsFile)
{
    TSettings settings;
    QSettings settingsINI(settingsFile,QSettings::IniFormat);
    settings.hostname = settingsINI.value("Network/hostname").toString();
    settings.port = settingsINI.value("Network/port").toInt();
    settings.bindingkey = settingsINI.value("Network/bindingkey").toString();
    settings.exchange = settingsINI.value("Network/exchange").toString();
    return settings;
}

void setLog(QString settingsFile)
{
    QSettings settingsINI(settingsFile,QSettings::IniFormat);
    QString logPath = settingsINI.value("Logging/logPath").toString();
    QString logLvl = settingsINI.value("Logging/logLevel").toString();
    g_logFile.reset(new QFile(logPath));
    g_logFile.data()->open(QFile::Append | QFile::Text);
    g_logLvl = logLvl;
    qInstallMessageHandler(messageHandler);
}
