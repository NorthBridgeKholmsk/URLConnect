#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <windows.h>
#include <shellapi.h>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QInputDialog>
#include <cstdlib>
#include "localserver.h"
#include "passworkapi.h"
#include "settingswindow.h"
#include "ad_control_plugin_interface.h"
#include "plugins.h"
#include "ui_plugins.h"

class CommandHandler: public QObject
{
    Q_OBJECT
public:
    CommandHandler(LocalServer& localserver);

public slots:
    void runApp(const QString& host, const QString& protocol, const QString& hostname);

private:
    void runAsAdmin(const QString& command);

    bool exeIsExsists(const QString& keyReg);
    QString PathShielding(const QString& path);
};

#endif // COMMANDHANDLER_H
