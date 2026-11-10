#ifndef SECURESTORAGE_H
#define SECURESTORAGE_H

#include <QByteArray>

//Шифрование чувствительных данных (например, ключей) средствами Windows DPAPI.
//Данные защищаются на уровне текущего пользователя: расшифровать их может только тот же пользователь ОС.
namespace SecureStorage {
    //Возвращает зашифрованный блок; пустой QByteArray при ошибке или пустых входных данных
    QByteArray protect(const QByteArray& data);
    //Возвращает исходные данные; пустой QByteArray при ошибке или пустых входных данных
    QByteArray unprotect(const QByteArray& data);
}

#endif // SECURESTORAGE_H