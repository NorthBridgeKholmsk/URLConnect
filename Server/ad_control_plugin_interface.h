#ifndef AD_CONTROL_PLUGIN_INTERFACE_H
#define AD_CONTROL_PLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QDialog>

class ADControlPluginInterface {
public:
    virtual ~ADControlPluginInterface(){}
    virtual QString adcontrolRunApp(const QDialog& parrent, const QString &host, const QString &protocol) = 0;
};

Q_DECLARE_INTERFACE(ADControlPluginInterface, "NorthBridge.ADControlPlugin/0.0.1")
#endif // AD_CONTROL_PLUGIN_INTERFACE_H
