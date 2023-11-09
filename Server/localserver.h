#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>

class LocalServer: public QObject
{
    Q_OBJECT
public:
    LocalServer(const QString& id, QObject *parent = nullptr);

signals:
    void dataReceived(const QString& host, const QString& protocol, const QString& idPass);

public slots:
    virtual void slotNewConnection();
    void slotReadClient();

private:
    void sendToClient(QLocalSocket* localSocket, const QString& string);
    QLocalServer* localServer;
    QLocalSocket* localSocket;
    quint16 nextBlockSize;
    QString host, protocol, idPass;
};

#endif // LOCALSERVER_H
