#include "localclient.h"

LocalClient::LocalClient(const QString& serverID, QObject *parent): QObject{parent}, nextBlockSize(0){
    localSocket = new QLocalSocket(this);
    connect(localSocket, &QLocalSocket::errorOccurred ,this, &LocalClient::slotError);
    localSocket->setServerName(serverID);
    connect(localSocket, &QLocalSocket::connected, this, [&](){this->slotConnected();});
    connect(localSocket, &QLocalSocket::readyRead, this, [&](){this->slotReadyRead();});
    localSocket->connectToServer();
}

void LocalClient::slotReadyRead(){
    QDataStream inputDataStream(localSocket);
    inputDataStream.setVersion(QDataStream::Qt_6_2);
    while(true){
        if (!nextBlockSize){
            if (localSocket->bytesAvailable() < (int)sizeof(quint16)){
                break;
            }
        }
        inputDataStream >> nextBlockSize;
        if (localSocket->bytesAvailable() < nextBlockSize){
            break;
        }
        QString string;
        inputDataStream >> string;

        nextBlockSize = 0;
    }
}

void LocalClient::slotError(QLocalSocket::LocalSocketError error){
    QString strError = "Error: " + (error == QLocalSocket::ServerNotFoundError ?
                       "The server was not found." :
                       error == QLocalSocket::PeerClosedError ?
                       "The server is closed." :
                       error == QLocalSocket::ConnectionRefusedError ?
                       "The connection was refused." :
                       QString(localSocket->errorString()));
}

void LocalClient::sendToServer(){
    QByteArray bits;
    QDataStream outputDataStream(&bits, QIODevice::WriteOnly);
    outputDataStream.setVersion(QDataStream::Qt_6_2);
    QString message;
    outputDataStream << quint16(0) << message;

    outputDataStream.device()->seek(0);
    outputDataStream << quint16(bits.size() - sizeof(quint16));

    localSocket->write(bits);
}

void LocalClient::slotConnected(){

}
