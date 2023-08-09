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
        qInfo() << "Открыто окно настройки сервера";
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
    qInfo() << "Добавлен значок программы в системный трей";
}

//Функция, добавляющее меню управления значку в трее
void URLConnectServer::setTrayIconActions(){
    openSettingsAction = new QAction("Настройки", this);
    logAction = new QAction("Логи", this);
    exitAction = new QAction("Выход", this);

    connect(openSettingsAction, &QAction::triggered, this, [&](){
        qInfo() << "Открыто окно настройки сервера";
        settings->show();
    });
    connect(logAction, &QAction::triggered, this, [&](){
        qInfo() << "Открыто окно просмотра логов";
        LogViewer logviewer;
        logviewer.exec();
    });
    connect(exitAction, &QAction::triggered, this, [&](){
        qWarning() << "Завершение работы программы было вызвано нажатием на кнопку выхода";
        QApplication::exit();
    });

    trayIconMenu = new QMenu();
    trayIconMenu->addAction(openSettingsAction);
    trayIconMenu->addAction(logAction);
    trayIconMenu->addAction(exitAction);
}
