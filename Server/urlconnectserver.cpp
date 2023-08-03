#include "urlconnectserver.h"

URLConnectServer::URLConnectServer(QObject *parent): QObject{parent}{
    //Определение настроек для QSettings
    QCoreApplication::setOrganizationName("Северный мост");
    QCoreApplication::setApplicationName("URLConnect");

    //Добавление меню управления и отображение значка в трее
    setTrayIconActions();
    showTrayIcon();
}

//Функция для обработки взаимодействия пользователя со значком в трее
void URLConnectServer::trayIconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::DoubleClick: //Если был двойной клик по значку, то открывается окно с настройками
        settings->show();
        break;
    default:
        break;
    }
}

//Функция, создающая значок в трее
void URLConnectServer::showTrayIcon(){
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(QIcon(":/iconTray/TrayIcon.ico"));
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &URLConnectServer::trayIconActivated);
    trayIcon -> show();
}

//Функция, добавляющее меню управления значку в трее
void URLConnectServer::setTrayIconActions(){
    openSettingsAction = new QAction("Настройки", this);
    exitAction = new QAction("Выход", this);

    connect(openSettingsAction, &QAction::triggered, this, [&](){settings->show();});
    connect(exitAction, &QAction::triggered, this, &QApplication::exit);

    trayIconMenu = new QMenu();
    trayIconMenu->addAction(openSettingsAction);
    trayIconMenu->addAction(exitAction);
}
