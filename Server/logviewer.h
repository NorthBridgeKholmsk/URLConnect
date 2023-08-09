#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QMessageBox>
#include "../Logger/logger.h"

namespace Ui {
class LogViewer;
}

class LogViewer : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = nullptr);
    ~LogViewer();

private slots:
    void on_closeButton_clicked();
    void on_updateLogViewer_clicked();

    void on_delLog_clicked();

private:
    void readAndViewLogFile();
    Ui::LogViewer *ui;
};

#endif // LOGVIEWER_H
