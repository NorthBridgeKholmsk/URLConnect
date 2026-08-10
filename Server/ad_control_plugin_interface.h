#ifndef AD_CONTROL_PLUGIN_INTERFACE_H
#define AD_CONTROL_PLUGIN_INTERFACE_H

#include <QtPlugin>

class ADControlPluginInterface {
public:
    virtual ~ADControlPluginInterface(){}
    virtual QString adcontrolRunApp(const QString &host, const QString &protocol, const QString &login){};
};

Q_DECLARE_INTERFACE(ADControlPluginInterface, "NorthBridge.ADControlPlugin/0.0.1")
#endif // AD_CONTROL_PLUGIN_INTERFACE_H
