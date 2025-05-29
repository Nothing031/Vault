#include "FileHeader.hpp"

#include <openssl/rand.h>

void FileHeader::RandIv()
{
    iv.resize(Sizes::iv);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), Sizes::iv);
}

FileInfo::State FileHeader::Deserialize(const QByteArray &data)
{
    if (data.size() < Sizes::total)
        return FileInfo::UNKNOWN_NONE;

    if (memcmp(signature.constData(),
               data.mid(Offsets::signature, Sizes::signature).constData(),
               Sizes::signature) != 0)
        return FileInfo::UNKNOWN_SIGNATURENOTMATCH;

    version = data.mid(Offsets::version, Sizes::version);
    salt = data.mid(Offsets::salt, Sizes::salt);
    memcpy(&iteration, data.constData() + Offsets::iteration, Sizes::iteration);
    hmac = data.mid(Offsets::hmac, Sizes::hmac);
    iv = data.mid(Offsets::iv, Sizes::iv);
    return FileInfo::CIPHER_GOOD;
}

FileInfo::State FileHeader::Deserialize(const QByteArray &data, const QByteArray &hmac)
{
    if (data.size() < Sizes::total)
        return FileInfo::UNKNOWN_NONE;

    if (memcmp(signature.constData(),
               data.mid(Offsets::signature, Sizes::signature).constData(),
               Sizes::signature) != 0)
        return FileInfo::UNKNOWN_SIGNATURENOTMATCH;

    if (hmac != data.mid(Offsets::hmac, Sizes::hmac)){
        return FileInfo::CIPHER_HEADERNOTMATCH;
    }

    this->hmac = hmac;
    version = data.mid(Offsets::version, Sizes::version);
    salt = data.mid(Offsets::salt, Sizes::salt);
    memcpy(&iteration, data.constData() + Offsets::iteration, Sizes::iteration);
    iv = data.mid(Offsets::iv, Sizes::iv);
    return FileInfo::CIPHER_GOOD;
}

QByteArray FileHeader::Serialize() const {
    QByteArray data(Sizes::total, 0);
    memcpy(data.data() + Offsets::signature,  signature,           Sizes::signature);
    memcpy(data.data() + Offsets::version,    version.constData(), Sizes::version);
    memcpy(data.data() + Offsets::salt,       salt.constData(),    Sizes::salt);
    memcpy(data.data() + Offsets::iteration,  &iteration,          Sizes::iteration);
    memcpy(data.data() + Offsets::hmac,       hmac.constData(),    Sizes::hmac);
    memcpy(data.data() + Offsets::iv,         iv.constData(),      Sizes::iv);
    return data;
}

void FileHeader::SetData(const AES256Settings& aes)
{
    this->version = aes.FormatVersion();
    this->salt = aes.GlobalSalt();
    this->iteration = aes.Iteration();
    this->hmac = aes.Hmac();
}
