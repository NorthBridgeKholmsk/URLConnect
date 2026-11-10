#ifndef PASSWORKPLUGIN_H
#define PASSWORKPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "../../Server/passwork_plugin_interface.h"

class QNetworkAccessManager;

//Плагин получения логина и пароля из хранилища Passwork.
class PassworkPlugin : public QObject, public PassworkPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NorthBridge.PassworkPlugin/0.0.1")
    Q_INTERFACES(PassworkPluginInterface)
public:
    explicit PassworkPlugin(QObject *parent = nullptr);

    bool getCredentials(const QString& apiKey, const QString& hostname,
                        const QString& protocol, QString& login, QString& pass) override;

private:
    QNetworkAccessManager* m_manager;
};

#endif // PASSWORKPLUGIN_H
