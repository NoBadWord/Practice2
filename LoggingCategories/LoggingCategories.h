#ifndef LOGGER_H
#define LOGGER_H

#include <QLoggingCategory>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QDebug>

Q_DECLARE_LOGGING_CATEGORY(logDebug)
Q_DECLARE_LOGGING_CATEGORY(logInfo)
Q_DECLARE_LOGGING_CATEGORY(logWarning)
Q_DECLARE_LOGGING_CATEGORY(logCritical)

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // LOGGER_H
