#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QDir>
#include <QFile>

class Logger
{
public:
    Logger();
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);
    const QString logFilePath = QDir::toNativeSeparators(QDir::tempPath()) + "\\UrlConnect\\log.txt";
};

#endif // LOGGER_H
