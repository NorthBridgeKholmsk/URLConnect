#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <windows.h>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include "localserver.h"
#include "passworkapi.h"

class CommandHandler: public QObject
{
    Q_OBJECT
public:
    CommandHandler(LocalServer& localserver);

public slots:
    void runApp(const QString& host, const QString& protocol, const QString& hostname);

private:
    bool exeIsExsists(const QString& keyReg);
    QString PathShielding(const QString& path);
};

#endif // COMMANDHANDLER_H
