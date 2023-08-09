#include "logviewer.h"
#include "ui_logviewer.h"

LogViewer::LogViewer(QWidget *parent) :QDialog(parent), ui(new Ui::LogViewer){
    ui->setupUi(this);
    //Убираем кнопку закрытия окна, окно закрывается только кнопкой "Закрыть"
    this->setWindowFlag(Qt::WindowCloseButtonHint, false);
    readAndViewLogFile();
}

LogViewer::~LogViewer(){
    delete ui;
}

//Обработчик нажатия на кнопку "Закрыть"
void LogViewer::on_closeButton_clicked(){
    this->hide();
    qInfo() << "Окно просмотра логов закрыто";
}

//Обработчик нажатия на кнопку "Обновить"
void LogViewer::on_updateLogViewer_clicked(){
    readAndViewLogFile();
}

//Функция, считывающая файл логов и отображающая данные в окне
void LogViewer::readAndViewLogFile(){
    QFile logFile(Logger().logFilePath);
    if (logFile.open(QIODevice::ReadOnly)){
        qInfo() << "Файл лога прочитан";
        ui->logBrowser->setText(logFile.readAll());
    }
    else {
        QMessageBox msg;
        msg.setWindowTitle("Просмотр логов");
        msg.setTextFormat(Qt::RichText);
        msg.setText("<table><tr>"
                    "<td><img src=\":/iconMsg/error.png\" width=\"40\" height=\"40\"></td>"
                    "<td><p style=\"vertical-align:middle\">Файл лога не может быть прочитан по причине: " + logFile.errorString() + "</p></td>"
                    "</tr></table>");
        msg.exec();
        qCritical() << "Файл лога не может быть прочитан по причине: " + logFile.errorString();
    }
    logFile.close();
}

//Обработчик нажатия на кнопку "Очистить"
void LogViewer::on_delLog_clicked(){
    if (QFile().exists(Logger().logFilePath)){
        QMessageBox msg;
        msg.setWindowTitle("Очистка логов");
        QPushButton* yesButton = msg.addButton("Да", QMessageBox::ButtonRole::YesRole);
        connect(yesButton, &QPushButton::clicked, this, [&](){
            QFile().remove(Logger().logFilePath);
            ui->logBrowser->clear();
        });
        msg.addButton("Нет", QMessageBox::ButtonRole::NoRole);
        msg.setText("Вы уверены, что хотите удалить файл логов?");
        msg.exec();
    }
}

