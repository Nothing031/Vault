#pragma once

#include <QByteArray>
#include <QJsonObject>

class AES256Settings
{
public:
    static AES256Settings FromJsonObject(const QJsonObject&);
    QJsonObject ToJsonObject();

    AES256Settings();

    bool GetEnabled() const;
    void SetEnabled(bool newEnabled);
    QByteArray GetFormatVersion() const;
    void SetFormatVersion(const QByteArray &newFormatVersion);
    QByteArray GetGlobalSalt() const;
    void SetGlobalSalt(const QByteArray &newGlobalSalt);
    int GetIteration() const;
    void SetIteration(int newIteration);
    QByteArray GetHmac() const;
    void SetHmac(const QByteArray &newHmac);
    bool GetIsUnlocked() const;
    void SetIsUnlocked(bool newIsUnlocked);
    QByteArray GetAesKey() const;
    void SetAesKey(const QByteArray &newAesKey);

private:
    bool       isUnlocked;
    QByteArray aesKey;
    bool       enabled;
    QByteArray formatVersion;
    QByteArray globalSalt;
    int        iteration;
    QByteArray hmac;
};

