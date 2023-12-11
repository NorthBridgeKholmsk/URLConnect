#ifndef URLCONNECTSERVER_H
#define URLCONNECTSERVER_H

#include <QApplication>
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include "settingswindow.h"
#include "../Logger/logger.h"
#include "logviewer.h"
#include "updater.h"

class URLConnectServer : public QObject
{
public:
    explicit URLConnectServer(QObject *parent = nullptr);
    void showTrayIcon();
    void setTrayIconActions();
signals:

public slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    QMenu* trayIconMenu;
    QAction* openSettingsAction;
    QAction* updateAction;
    QAction* logAction;
    QAction* exitAction;

    SettingsWindow* settings = new SettingsWindow();

    void checkUpdate();
};

#endif // URLCONNECTSERVER_H
