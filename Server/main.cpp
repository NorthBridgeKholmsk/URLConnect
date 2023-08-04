#include <QApplication>
#include <QDateTime>
#include <QRegularExpression>
#include "urlconnectserver.h"

//Обработчик вывода логов в файл
void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg){
    QString string = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"); //Добавление даты и времени в строку лога

    switch (type) { //Добавление типа сообщения в строку лога
    case QtInfoMsg:
        string += " Info:";
        break;
     case QtDebugMsg:
        string += " Debug:";
        break;
     case QtWarningMsg:
        string += " Warning:";
        break;
     case QtCriticalMsg:
        string += " CriticalError:";
        break;
     case QtFatalMsg:
        string += " FatalError:";
        break;
    default:
        break;
    }
    string += " " + QString(context.file) + " " + QString(context.function) + " -> " + msg; //Добавление файла, функции и сообщения
    QFile fileLog(QApplication::applicationDirPath() + "\\log.txt");
    fileLog.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&fileLog);
    ts << string << Qt::endl;
    fileLog.close();
}

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qInstallMessageHandler(myMessageHandler);
    qInfo() << "Запуск программы";
    URLConnectServer cs;
    return a.exec();
}
