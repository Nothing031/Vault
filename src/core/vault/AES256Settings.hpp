#pragma once

#include <QByteArray>
#include <QJsonObject>

class AES256Settings
{
public:
    static AES256Settings FromJsonObject(const QJsonObject&);
    QJsonObject ToJsonObject();

    AES256Settings();

    bool IsEnabled() const;
    void SetEnabled(bool newEnabled);
    QByteArray FormatVersion() const;
    void SetFormatVersion(const QByteArray &newFormatVersion);
    QByteArray GlobalSalt() const;
    void SetGlobalSalt(const QByteArray &newGlobalSalt);
    int Iteration() const;
    void SetIteration(int newIteration);
    QByteArray Hmac() const;
    void SetHmac(const QByteArray &newHmac);
    bool IsLocked() const;
    void Unlock();
    void Lock();
    QByteArray AesKey() const;
    void SetAesKey(const QByteArray &newAesKey);

private:
    bool       locked;
    QByteArray aesKey;
    bool       enabled;
    QByteArray formatVersion;
    QByteArray globalSalt;
    int        iteration;
    QByteArray hmac;
};

