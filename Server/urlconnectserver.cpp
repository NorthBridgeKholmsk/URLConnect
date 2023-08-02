#include "urlconnectserver.h"

URLConnectServer::URLConnectServer(QObject *parent): QObject{parent}{
    setTrayIconActions();
    showTrayIcon();
}

void URLConnectServer::trayIconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::Context:

        break;
    default:
        break;
    }
}

void URLConnectServer::showTrayIcon(){
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(QIcon(":/iconTray/TrayIcon.ico"));
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &URLConnectServer::trayIconActivated);
    trayIcon -> show();
}

void URLConnectServer::setTrayIconActions(){
    openSettingsAction = new QAction("Настройки", this);
    exitAction = new QAction("Выход", this);

    connect(exitAction, &QAction::triggered, this, &QApplication::exit);

    trayIconMenu = new QMenu();
    trayIconMenu->addAction(openSettingsAction);
    trayIconMenu->addAction(exitAction);
}
