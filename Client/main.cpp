#include <QCoreApplication>
#include <QSettings>
#include "localclient.h"
#include "parserurl.h"
#include "../Logger/logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMessageHandler(Logger::messageHandler);
    qInfo() << "Запуск клиента";
    if (argc > 1){
        ParserURL arg(argv[1]);
        LocalClient lc(QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Северный мост\\URLConnect\\settings", QSettings::NativeFormat).value("port").toString(),
                       arg.getHost(), arg.getProtocol(), arg.getIdPass());
    }
    else {
        qCritical() << "Клиент не может быть запущен так как не был передан URL в качестве аргумента";
        return 0;
    }
    return a.exec();
}
