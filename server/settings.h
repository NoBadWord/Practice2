#ifndef TSETTINGS_H
#define TSETTINGS_H

#include "LoggingCategories.h"

#include <QString>
#include <QSettings>
#include <QFile>

extern QScopedPointer<QFile> g_logFile;
extern QString g_logLvl;

struct TSettings
{
    QString hostname;
    QString bindingkey;
    QString exchange;
    int port;
};

TSettings readSettings(QString settingsFile);

void setLog(QString settingsFile);

#endif // TSETTINGS_H
