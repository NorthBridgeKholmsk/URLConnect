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
#include "passwork_plugin_interface.h"
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
    //Запуск консольной команды (ping/traceroute) в отдельном окне через shell
    void launchConsoleCommand(const QString& title, const QString& command);

    bool exeExists(const QString& keyReg);
    QString PathShielding(const QString& path);

    //Получение учётных данных из плагина Passwork (true при успехе)
    bool getPassworkCredentials(const QByteArray& plainApiKey, const QString& hostname,
                                const QString& protocol, QString& login, QString& pass);
    //Учётные данные RADIUS: radiusLogin + суффикс протокола + TOTP 2FA
    void getRadiusCredentials(const QString& protocol, QString& login, QString& pass);
};

#endif // COMMANDHANDLER_H
