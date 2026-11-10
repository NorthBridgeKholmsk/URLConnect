#ifndef AUTHMETHODDIALOG_H
#define AUTHMETHODDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AuthMethodDialog; }
QT_END_NAMESPACE

//Диалог выбора метода аутентификации для протоколов ssh/ciscossh/winbox.
class AuthMethodDialog : public QDialog
{
    Q_OBJECT
public:
    enum Method { Manual, Passwork, Radius };

    explicit AuthMethodDialog(const QString& defaultHostname = QString(), bool passworkAvailable = false, QWidget *parent = nullptr);
    ~AuthMethodDialog();

    Method method() const;
    //Ручной ввод (метод Manual)
    QString login() const;
    QString password() const;
    //Имя хоста для поиска в Passwork (метод Passwork)
    QString hostname() const;

private slots:
    void onMethodChanged();

private:
    Ui::AuthMethodDialog *ui;
};

#endif // AUTHMETHODDIALOG_H