#include "parserurl.h"

ParserURL::ParserURL(const QString& arg, QObject *parent): QObject{parent}{
    QStringList _arg = arg.split(':').at(1).split('+');
    host = _arg.at(0);
    protocol = _arg.at(1);
    if (_arg.size() > 2)
        hostname = _arg.at(2);
    else
        hostname = "0";
    qInfo() << "Клиент разобрал полученый URL";
}

QString ParserURL::getProtocol() const{
    return protocol;
}

QString ParserURL::getHost() const{
    return host;
}

QString ParserURL::getHostname() const{
    return hostname;
}
