#include "AES256Settings.hpp"

#include "src/core/FileInfo.hpp"
#include "src/core/cryptography/Cryptography.hpp"

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
    object["FormatVersion"] = QString::fromLocal8Bit(formatVersion);
    object["GlobalSalt"] = QString(globalSalt.toBase64());
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

bool AES256Settings::IsEnabled() const { return enabled; }
void AES256Settings::SetEnabled(bool newEnabled) { enabled = newEnabled; }
void AES256Settings::SetFormatVersion(const QByteArray &newFormatVersion) { formatVersion = newFormatVersion; }
QByteArray AES256Settings::FormatVersion() const { return formatVersion; }
void AES256Settings::SetGlobalSalt(const QByteArray &newGlobalSalt) { globalSalt = newGlobalSalt; }
QByteArray AES256Settings::GlobalSalt() const { return globalSalt; }
void AES256Settings::SetIteration(int newIteration) { iteration = newIteration; }
int AES256Settings::Iteration() const { return iteration; }
void AES256Settings::SetHmac(const QByteArray &newHmac) { hmac = newHmac; }
bool AES256Settings::IsLocked() const { return locked; }
bool AES256Settings::TryUnlock(const QString& input) {
    QByteArray genKey = Cryptography::GenerateKey(input, globalSalt, iteration);
    if (hmac == genKey.mid(0, 32)){
        aesKey = genKey.mid(32, 32);
        locked = false;
        return true;
    }else{
        return false;
    }
}
void AES256Settings::Lock() {
    this->locked = true;
    aesKey.clear();
}
QByteArray AES256Settings::AesKey() const { return aesKey; }
void AES256Settings::SetAesKey(const QByteArray &newAesKey) { aesKey = newAesKey; }
QByteArray AES256Settings::Hmac() const { return hmac; }

