#ifndef LOCACLIENT_H
#define LOCACLIENT_H

#include <QObject>
#include <QSettings>
#include <QLocalSocket>
#include <QCoreApplication>
#include "../Logger/logger.h"

class LocalClient : public QObject
{
public:
    explicit LocalClient(const QString& serverID,
                         const QString& _host,
                         const QString& _protocol,
                         const QString& _hostname,
                         QObject *parent = nullptr);
    void sendToServer();

private:
    QLocalSocket* localSocket;
    quint16 nextBlockSize;
    const QString host, protocol, hostname;

private slots:
    void slotReadyRead();
    void slotError(QLocalSocket::LocalSocketError error);
};

#endif // LOCACLIENT_H
