#ifndef ADCONTROLPLUGIN_H
#define ADCONTROLPLUGIN_H

#include <QObject>
#include <QtPlugin>
//#include <QDialog>
#include "../URLConnect/Server/settingswindow.h"
#include <QInputDialog>
#include "../URLConnect/Server/ad_control_plugin_interface.h"

class ADControlPlugin : public QObject, public ADControlPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NorthBridge.ADControlPlugin/0.0.1")
    Q_INTERFACES(ADControlPluginInterface)
public:
    explicit ADControlPlugin(QObject *parent = nullptr);
    QString adcontrolRunApp(const QString &host, const QString &protocol, const QString &login) override;
signals:

};

#endif // ADCONTROLPLUGIN_H
