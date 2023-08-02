#include <QApplication>
#include "urlconnectserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    URLConnectServer cs;
    return a.exec();
}
