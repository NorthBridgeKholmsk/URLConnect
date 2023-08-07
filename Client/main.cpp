#include <QCoreApplication>
#include "localclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    LocalClient lc("");
    return a.exec();
}
