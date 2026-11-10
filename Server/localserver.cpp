#include "localserver.h"
#include <QUuid>
#include <QSettings>

//Загружает токен аутентификации локального пайпа; при первом запуске создаёт и сохраняет его в реестре текущего пользователя
static QString loadOrCreatePipeToken(){
    QSettings auth("URLConnect", "PipeAuth");
    QString token = auth.value("token").toString();
    if (token.isEmpty()){
        token = QUuid::createUuid().toString(QUuid::WithoutBraces);
        auth.setValue("token", token);
        auth.sync();
        qInfo() << "Создан новый токен аутентификации локального пайпа";
    }
    return token;
}

LocalServer::LocalServer(const QString& id, QObject *parent): QObject{parent}, nextBlockSize(0){
    localServer = new QLocalServer();
    if (!localServer->listen(id)){
        qCritical() << "Локальный сервер не может быть запущен, по причине: " + localServer->errorString() + " Для повторной попытки необходимо перезапустить программу";
        localServer->close();
        return;
    }
    pipeToken = loadOrCreatePipeToken();
    qInfo() << "Локальный сервер запущен с идентификатором " + id;
    connect(localServer, &QLocalServer::newConnection, this, [&](){
        qInfo() << "Клиент подключился к локальному серверу";
        this->slotNewConnection();
    });
}

void LocalServer::slotNewConnection(){
    localSocket = localServer->nextPendingConnection();
    connect(localSocket, &QLocalSocket::disconnected, localSocket, [&](){
        //&QLocalSocket::deleteLater;
        qInfo() << "Клиент отключился от сервера";
        //localSocket->disconnectFromServer();
    });
    connect(localSocket, &QLocalSocket::readyRead, localSocket, [&](){this->slotReadClient();});
    localSocket->write("msgRcvd1");
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
        QString receivedToken;
        inputDataStream >> host >> protocol >> idPass >> receivedToken;
        nextBlockSize = 0;
        if (receivedToken != pipeToken){
            qCritical() << "Отклонён запрос с неверным токеном аутентификации локального пайпа";
            sendToClient(localSocket, "authFailed");
            return;
        }
        qInfo() << "Данные от клиента получены";
        sendToClient(localSocket, "msgRcvd1");
        emit dataReceived(host, protocol, idPass);
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
