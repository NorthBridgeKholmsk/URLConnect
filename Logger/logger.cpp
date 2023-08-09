#include "logger.h"

Logger::Logger(){}

//Обработчик вывода логов в файл
void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg){
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
    //Если путь к папке с файлом лога отсутствует, то создать его
    QString dirPath = QFileInfo(Logger().logFilePath).dir().path();
    if (!QDir().exists(dirPath)){
        QDir().mkpath(dirPath);
    }
    //Запись лога в файл
    QFile fileLog(Logger().logFilePath);
    fileLog.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&fileLog);
    ts << string << Qt::endl;
    fileLog.close();
}
