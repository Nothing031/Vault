#include "FileInfoHeader.hpp"

#include <openssl/rand.h>

void FileInfoHeader::RandIv()
{
    iv.resize(Sizes::iv);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), Sizes::iv);
}

bool FileInfoHeader::TrySetData(const QByteArray &data)
{
    if (data.size() < Sizes::total)
        return false;

    if (memcmp(signature.constData(),
               data.mid(Offsets::signature, Sizes::signature).constData(),
               Sizes::signature) != 0)
        return false;

    version = data.mid(Offsets::version, Sizes::version);
    salt = data.mid(Offsets::salt, Sizes::salt);
    memcpy(&iteration, data.constData() + Offsets::iteration, Sizes::iteration);
    hmac = data.mid(Offsets::hmac, Sizes::hmac);
    iv = data.mid(Offsets::iv, Sizes::iv);
    return true;
}

QByteArray FileInfoHeader::GetData() const {
    QByteArray data(Sizes::total, 0);
    memcpy(data.data() + Offsets::signature,  signature,           Sizes::signature);
    memcpy(data.data() + Offsets::version,    version.constData(), Sizes::version);
    memcpy(data.data() + Offsets::salt,       salt.constData(),    Sizes::salt);
    memcpy(data.data() + Offsets::iteration,  &iteration,          Sizes::iteration);
    memcpy(data.data() + Offsets::hmac,       hmac.constData(),    Sizes::hmac);
    memcpy(data.data() + Offsets::iv,         iv.constData(),      Sizes::iv);
    return data;
}

void FileInfoHeader::SetHeader(const QByteArray &version, const QByteArray &salt, const int &iteration, const QByteArray &hmac)
{
    this->version = version;
    this->salt = salt;
    this->iteration = iteration;
    this->hmac = hmac;
}
