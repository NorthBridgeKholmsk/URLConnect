#include "localserver.h"

LocalServer::LocalServer(const QString& id, QObject *parent): QObject{parent}, nextBlockSize(0){
    localServer = new QLocalServer(this);
    if (!localServer->listen(id)){
        qCritical() << "Локальный сервер не может быть запущен, по причине: " + localServer->errorString() + " Для повторной попытки необходимо перезапустить программу";
        localServer->close();
        return;
    }
    qInfo() << "Локальный сервер запущен с идентификатором " + id;
    connect(localServer, &QLocalServer::newConnection, this, [&](){
        qInfo() << "Клиент подключился к локальному серверу";
        this->slotNewConnection();
    });
}

void LocalServer::slotNewConnection(){
    localSocket = localServer->nextPendingConnection();
    connect(localSocket, &QLocalSocket::disconnected, localSocket, &QLocalSocket::deleteLater);
    connect(localSocket, &QLocalSocket::readyRead, localSocket, [&](){slotReadClient();});
}

void LocalServer::slotReadClient(){
    QDataStream inputDataStream(localSocket);
    inputDataStream.setVersion(QDataStream::Qt_6_2);
    qInfo() << "Сервер начал прием данных от клиента";
    while (true){
        if (!nextBlockSize){
            if (localSocket->bytesAvailable() < (int)sizeof(quint16)){
                break;
            }
            inputDataStream >> nextBlockSize;
        }
        inputDataStream >> host >> protocol >> idPass;
        nextBlockSize = 0;
        qInfo() << "Данные от клиента получены";
        sendToClient(localSocket, "msgRcvd1");
    }
}

void LocalServer::sendToClient(QLocalSocket *localSocket, const QString &string){
    QByteArray bits;
    QDataStream outputDataStream(&bits, QIODevice::WriteOnly);
    outputDataStream.setVersion(QDataStream::Qt_6_2);

    outputDataStream << quint16(0) << string;
    outputDataStream.device()->seek(0);
    outputDataStream << quint16(bits.size() - sizeof(quint16));
    localSocket->write(bits);
    qInfo() << "Сервер отправил клиенту код " + string;
}
