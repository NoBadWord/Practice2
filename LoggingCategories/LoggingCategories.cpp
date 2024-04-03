#include "LoggingCategories.h"

Q_LOGGING_CATEGORY(logDebug,    "Debug")
Q_LOGGING_CATEGORY(logInfo,     "Info")
Q_LOGGING_CATEGORY(logWarning,  "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")

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
