#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QDir>
#include <QLineEdit>
#include <QFileDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();

public slots:
    void findExeFile(QLineEdit* line, const QString& title);

private:
    void setSettingsToRegistr();
    void getSettingsFromRegistr();
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
