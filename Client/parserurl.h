#ifndef PARSERURL_H
#define PARSERURL_H

#include <QObject>
#include "../Logger/logger.h"

class ParserURL : public QObject{
public:
    explicit ParserURL(const QString& arg, QObject *parent = nullptr);
    QString getProtocol() const;
    QString getHost() const;
    QString getIdPass() const;

private:
    QString protocol;
    QString host;
    QString idPass;
};

#endif // PARSERURL_H
