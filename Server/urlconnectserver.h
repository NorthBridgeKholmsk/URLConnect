#ifndef URLCONNECTSERVER_H
#define URLCONNECTSERVER_H

#include <QApplication>
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QObject>

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
    QAction* exitAction;
};

#endif // URLCONNECTSERVER_H
