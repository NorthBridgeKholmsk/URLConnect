#ifndef LOCACLIENT_H
#define LOCACLIENT_H

#include <QObject>
#include <QSettings>
#include <QLocalSocket>

class LocalClient : public QObject
{
public:
    explicit LocalClient(const QString& serverID, QObject *parent = nullptr);
    void sendToServer();

private:
    QLocalSocket* localSocket;
    quint16 nextBlockSize;

private slots:
    void slotReadyRead();
    void slotError(QLocalSocket::LocalSocketError error);

    void slotConnected();
};

#endif // LOCACLIENT_H
