#include "settings.h"

TSettings::TSettings()
{
    updateSettings();
}

void TSettings::updateSettings()
{
    QSettings settings(m_settingsFile,QSettings::IniFormat);
    m_logPath = settings.value("Logging/logPath").toString();
    m_logLvl = settings.value("Logging/logLevel").toString();
    m_hostname = settings.value("Network/hostname").toString();
    m_port = settings.value("Network/port").toInt();
    m_bindingkey = settings.value("Network/bindingkey").toString();
    m_exchange = settings.value("Network/exchange").toString();
}

void TSettings::setSettingsFile(QString settingsFile)
{
    m_settingsFile = settingsFile;
    updateSettings();
}

QString TSettings::logPath(){return m_logPath;}
QString TSettings::logLvl(){return m_logLvl;}
QString TSettings::hostname(){return m_hostname;}
QString TSettings::bindingkey(){return m_bindingkey;}
QString TSettings::exchange(){return m_exchange;}
int TSettings::port(){return m_port;}
