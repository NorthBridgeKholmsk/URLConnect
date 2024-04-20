#include "localclient.h"

LocalClient::LocalClient(const QString& serverID, const QString& _host, const QString& _protocol, const QString& _hostname, QObject *parent):
    QObject{parent}, nextBlockSize(0), host(_host), protocol(_protocol), hostname(_hostname){
    localSocket = new QLocalSocket();
    connect(localSocket, &QLocalSocket::errorOccurred ,localSocket, [&](){
        this->slotError(localSocket->error());
        QCoreApplication::exit();
    }, Qt::QueuedConnection);
    localSocket->setServerName(serverID);
    connect(localSocket, &QLocalSocket::connected, this, [&](){
        qInfo() << "Клиент подключился к серверу";
        this->sendToServer();
    });
    connect(localSocket, &QLocalSocket::readyRead, localSocket, [&](){this->slotReadyRead();});
    qInfo() << "Клиент начал подключение к серверу";
    localSocket->connectToServer();
}

void LocalClient::slotReadyRead(){
    qInfo() << "Клиент начал прием данных от сервера";
    QDataStream inputDataStream(localSocket);
    inputDataStream.setVersion(QDataStream::Qt_6_2);
    while(true){
        if (!nextBlockSize){
            if (localSocket->bytesAvailable() < (int)sizeof(quint16)){
                break;
            }
        }
        inputDataStream >> nextBlockSize;
        QString string;
        inputDataStream >> string;
        qInfo() << "Клиент получил данные от сервера";
        nextBlockSize = 0;

        if (string == "msgRcvd1"){
            qInfo() << "Клиент получил подтвержение от сервера об успешном получении данных";
            QCoreApplication::exit();
        }
    }
}

void LocalClient::slotError(QLocalSocket::LocalSocketError error){
    QString strError = "Ошибка: " + (error == QLocalSocket::ServerNotFoundError ?
                       "Сервер не найден." :
                       error == QLocalSocket::PeerClosedError ?
                       "Сервер закрыт." :
                       error == QLocalSocket::ConnectionRefusedError ?
                       "Соединение с сервер было сброшено." :
                       QString(localSocket->errorString()));
    qCritical() << "Клиент не может подключиться к серверу по причине: " + strError;
}

void LocalClient::sendToServer(){
    qInfo() << "Клиент начал передачу данных серверу";
    QByteArray bits;
    QDataStream outputDataStream(&bits, QIODevice::WriteOnly);
    outputDataStream.setVersion(QDataStream::Qt_6_2);
    outputDataStream << quint16(0) << host << protocol << hostname;

    outputDataStream.device()->seek(0);
    outputDataStream << quint16(bits.size() - sizeof(quint16));

    localSocket->write(bits);
    qInfo() << "Клиент закончил передачу данных серверу";
}
