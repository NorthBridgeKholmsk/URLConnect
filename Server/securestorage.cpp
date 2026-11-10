#include "securestorage.h"
#include <QDebug>
#include <windows.h>
#include <wincrypt.h>

namespace SecureStorage {

QByteArray protect(const QByteArray& data){
    if (data.isEmpty()) return {};
    DATA_BLOB inBlob = {}, outBlob = {};
    inBlob.pbData = const_cast<BYTE*>(reinterpret_cast<const BYTE*>(data.constData()));
    inBlob.cbData = static_cast<DWORD>(data.size());
    //CRYPTPROTECT_UI_FORBIDDEN — не показывать диалог Windows при шифровании
    if (!CryptProtectData(&inBlob, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &outBlob)){
        qWarning() << "DPAPI: не удалось зашифровать данные (код ошибки" << GetLastError() << ")";
        return {};
    }
    QByteArray result(reinterpret_cast<const char*>(outBlob.pbData), static_cast<int>(outBlob.cbData));
    LocalFree(outBlob.pbData);
    return result;
}

QByteArray unprotect(const QByteArray& data){
    if (data.isEmpty()) return {};
    DATA_BLOB inBlob = {}, outBlob = {};
    inBlob.pbData = const_cast<BYTE*>(reinterpret_cast<const BYTE*>(data.constData()));
    inBlob.cbData = static_cast<DWORD>(data.size());
    //CRYPTPROTECT_UI_FORBIDDEN — не показывать диалог Windows при расшифровании
    if (!CryptUnprotectData(&inBlob, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &outBlob)){
        qWarning() << "DPAPI: не удалось расшифровать данные (код ошибки" << GetLastError() << ")";
        return {};
    }
    QByteArray result(reinterpret_cast<const char*>(outBlob.pbData), static_cast<int>(outBlob.cbData));
    LocalFree(outBlob.pbData);
    return result;
}

} //namespace SecureStorage