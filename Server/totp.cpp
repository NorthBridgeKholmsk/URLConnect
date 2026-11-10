#include "totp.h"
#include <QCryptographicHash>
#include <QDateTime>

namespace {
    //Декодирование Base32-секрета (регистронезависимо; игнорирует пробелы и символ '='). Пустой QByteArray при отсутствии валидных символов.
    static QByteArray base32Decode(const QString& in){
        const QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
        QByteArray out;
        int bits = 0;          //сколько бит накоплено в value и ещё не извлечено как байты (всегда < 8)
        uint32_t value = 0;    //накопитель битов; маскируется после каждого извлечения, чтобы не переполняться
        for (QChar c : in){
            if (c == '=' || c.isSpace()) continue;
            const int idx = alphabet.indexOf(c.toUpper());
            if (idx < 0) continue; //игнорировать невалидные символы
            value = (value << 5) | static_cast<uint32_t>(idx);
            bits += 5;
            if (bits >= 8){
                bits -= 8;
                out.append(char((value >> bits) & 0xFF));
                //Убираем уже извлечённые старшие биты: в накопителе остаются только младшие `bits` бит
                value &= static_cast<uint32_t>((1u << bits) - 1);
            }
        }
        return out;
    }

    //HMAC-SHA1. Qt не имеет встроенного HMAC, поэтому реализуем его двумя вызовами QCryptographicHash (RFC 2104).
    static QByteArray hmacSha1(const QByteArray& key, const QByteArray& msg){
        const int blockSize = 64; //размер блока SHA-1 в байтах

        //Нормализуем ключ до размера блока: если длиннее — хешируем (SHA-1 даёт ровно 20 байт)
        QByteArray k;
        if (key.size() > blockSize){
            QCryptographicHash h(QCryptographicHash::Sha1);
            h.addData(key);
            k = h.result();
        } else {
            k = key;
        }
        //Явно дополняем нулями до размера блока (гарантированная инициализация каждого байта)
        while (k.size() < blockSize){
            k.append('\0');
        }

        //ipad/opad создаём сразу заполненными нужным значением, затем XOR-ируем с ключом
        QByteArray ipad(blockSize, char(0x36));
        QByteArray opad(blockSize, char(0x5C));
        for (int i = 0; i < blockSize; ++i){
            ipad[i] = char(ipad[i] ^ k[i]);
            opad[i] = char(opad[i] ^ k[i]);
        }

        QCryptographicHash inner(QCryptographicHash::Sha1);
        inner.addData(ipad);
        inner.addData(msg);
        const QByteArray innerHash = inner.result();

        QCryptographicHash outer(QCryptographicHash::Sha1);
        outer.addData(opad);
        outer.addData(innerHash);
        return outer.result();
    }

    //Ядро TOTP: вычисление кода по явному counter (dynamic truncation, RFC 4226)
    static QString computeCode(const QByteArray& key, quint64 counter, int digits){
        //Формируем 8-байтовое сообщение counter в big-endian порядке (RFC 4226)
        QByteArray msg(8, 0);
        for (int i = 7; i >= 0; --i){
            msg[i] = char(counter & 0xFF);
            counter >>= 8;
        }

        const QByteArray hmac = hmacSha1(key, msg);
        if (hmac.size() < 4) return {};

        const int offset = static_cast<int>(static_cast<quint8>(hmac.at(hmac.size() - 1))) & 0x0F;
        quint32 binary = (quint8)hmac[offset] & 0x7F;
        binary = (binary << 8) | (quint8)hmac[offset + 1];
        binary = (binary << 8) | (quint8)hmac[offset + 2];
        binary = (binary << 8) | (quint8)hmac[offset + 3];

        int mod = 1;
        for (int i = 0; i < digits; ++i) mod *= 10;
        const int otp = static_cast<int>(binary % quint32(mod));
        return QString::number(otp).rightJustified(digits, '0');
    }
}

namespace TOTP {
QString generateAtTime(const QString& base32Secret, quint64 unixSeconds, int period, int digits){
    if (period <= 0 || digits <= 0) return {};
    const QByteArray key = base32Decode(base32Secret);
    if (key.isEmpty()) return {};
    const quint64 counter = unixSeconds / static_cast<quint64>(period);
    return computeCode(key, counter, digits);
}

QString generate(const QString& base32Secret, int period, int digits){
    return generateAtTime(base32Secret, static_cast<quint64>(QDateTime::currentSecsSinceEpoch()), period, digits);
}
}
