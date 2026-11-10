#ifndef TOTP_H
#define TOTP_H

#include <QString>
#include <QtGlobal> //для quint64

//Генерация одноразовых кодов второго фактора по RFC 6238 (TOTP) — как в Google/Microsoft Authenticator.
namespace TOTP {
    //Возвращает код из `digits` цифр, сгенерированный по Base32-секрету на текущий момент; пустая строка при некорректном секрете
    QString generate(const QString& base32Secret, int period = 30, int digits = 6);

    //Генерация кода для явного момента времени (unix-секунды). Используется в тестах и отладке.
    QString generateAtTime(const QString& base32Secret, quint64 unixSeconds, int period = 30, int digits = 6);
}

#endif // TOTP_H
