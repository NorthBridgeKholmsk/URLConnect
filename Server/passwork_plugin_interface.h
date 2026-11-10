#ifndef PASSWORK_PLUGIN_INTERFACE_H
#define PASSWORK_PLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QString>

//Интерфейс плагина, предоставляющего доступ к хранилищу паролей Passwork.
class PassworkPluginInterface {
public:
    virtual ~PassworkPluginInterface(){}
    //Возвращает true при успешном получении данных; login и pass заполняются по ссылке.
    virtual bool getCredentials(const QString& apiKey, const QString& hostname,
                                const QString& protocol, QString& login, QString& pass) = 0;
};

Q_DECLARE_INTERFACE(PassworkPluginInterface, "NorthBridge.PassworkPlugin/0.0.1")
#endif // PASSWORK_PLUGIN_INTERFACE_H
