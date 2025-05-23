#include "AES256Settings.hpp"

#include "src/core/FileInfo.hpp"

AES256Settings AES256Settings::FromJsonObject(const QJsonObject &object)
{
    AES256Settings settings;
    settings.enabled = object["EncryptionEnabled"].toBool(false);
    settings.formatVersion = object["FormatVersion"].toString().toLocal8Bit();
    settings.globalSalt = QByteArray::fromBase64(object["GlobalSalt"].toString().toLatin1());
    settings.hmac = QByteArray::fromBase64(object["HMAC"].toString().toLatin1());
    settings.iteration = object["Iteration"].toInt(-1);
    return settings;
}

QJsonObject AES256Settings::ToJsonObject()
{
    QJsonObject object;
    object["EncryptionEnabled"] = enabled;
    object["FormatVersion"] = QString(formatVersion.toBase64());
    object["GlobalSalt"] = QString(formatVersion.toBase64());
    object["HMAC"] = QString(hmac.toBase64());
    object["Iteration"] = iteration;
    return object;
}

AES256Settings::AES256Settings() {
    FileInfo::FileHeader header;
    formatVersion = header.version;
    globalSalt = header.salt;
    iteration = header.iteration;
    hmac = header.hmac;
}

bool AES256Settings::GetEnabled() const { return enabled; }
void AES256Settings::SetEnabled(bool newEnabled) { enabled = newEnabled; }
void AES256Settings::SetFormatVersion(const QByteArray &newFormatVersion) { formatVersion = newFormatVersion; }
QByteArray AES256Settings::GetFormatVersion() const { return formatVersion; }
void AES256Settings::SetGlobalSalt(const QByteArray &newGlobalSalt) { globalSalt = newGlobalSalt; }
QByteArray AES256Settings::GetGlobalSalt() const { return globalSalt; }
void AES256Settings::SetIteration(int newIteration) { iteration = newIteration; }
int AES256Settings::GetIteration() const { return iteration; }
void AES256Settings::SetHmac(const QByteArray &newHmac) { hmac = newHmac; }
bool AES256Settings::GetIsUnlocked() const { return isUnlocked; }
void AES256Settings::SetIsUnlocked(bool newIsUnlocked) { isUnlocked = newIsUnlocked; }
QByteArray AES256Settings::GetAesKey() const { return aesKey; }
void AES256Settings::SetAesKey(const QByteArray &newAesKey) { aesKey = newAesKey; }
QByteArray AES256Settings::GetHmac() const { return hmac; }

