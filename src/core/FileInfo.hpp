#pragma once

#include <QString>
#include <QByteArray>
#include <QDebug>

#include <openssl/rand.h>

#define FORMAT_VERSION "0002"
#define SIGNATURE "VAULT BY NOTHING031"
#define ITERATION 100000
#define EXTENSION ".enc"

class FileInfo{
public:
    struct Sizes{
        static constexpr int signature = 0x16;
        static constexpr int version = 0x04;
        static constexpr int salt = 0x20;
        static constexpr int iteration = 0x04;
        static constexpr int hmac = 0x20;
        static constexpr int iv = 0x10;
        static constexpr int total = signature + version + salt + iteration + hmac + iv;
    };
    struct Offsets{
        static constexpr int signature = 0;
        static constexpr int version = signature + Sizes::signature;
        static constexpr int salt = version + Sizes::version;
        static constexpr int iteration = salt + Sizes::salt;
        static constexpr int hmac = iteration + Sizes::iteration;
        static constexpr int iv = hmac + Sizes::hmac;
    };
    struct FileHeader {
    private:
        inline static const QByteArray currentVersion = []{QByteArray data(Sizes::version, 0); memcpy(data.data(), FORMAT_VERSION, Sizes::version); return data;}();
    public:
        inline static const QByteArray signature = []{QByteArray data(Sizes::signature, 0); memcpy(data.data(), SIGNATURE, strlen(SIGNATURE)); return data;}();
        QByteArray version = currentVersion;
        QByteArray salt = {};
        int iteration = ITERATION;
        QByteArray hmac = {};
        QByteArray iv = {};
    };
    struct FilePath{
        QString absolutepath;
        QString relativePath;
        QString displayPath;
    };

    void ResetIv(){
        header.iv.resize(Sizes::iv);
        RAND_bytes(reinterpret_cast<unsigned char*>(header.iv.data()), Sizes::iv);
    }
    bool SetHeader(const QByteArray& data){
        if (data.size() < Sizes::total)
            return false;

        if (memcmp(header.signature.constData(),
                   data.mid(Offsets::signature, Sizes::signature).constData(),
                   Sizes::signature) != 0)
            return false;

        header.version = data.mid(Offsets::version, Sizes::version);
        header.salt = data.mid(Offsets::salt, Sizes::salt);
        memcpy(&header.iteration, data.constData() + Offsets::iteration, Sizes::iteration);
        header.hmac = data.mid(Offsets::hmac, Sizes::hmac);
        header.iv = data.mid(Offsets::iv, Sizes::iv);
        return true;
    }
    bool SetHeader(const QByteArray& version, const QByteArray& salt, const int& iteration, const QByteArray& hmac){
        header.version = version;
        header.salt = salt;
        header.iteration = iteration;
        header.hmac = hmac;
        return true;
    }
    QByteArray GetHeader() const {
        QByteArray data(Sizes::total, 0);
        memcpy(data.data() + Offsets::signature,  header.signature,           Sizes::signature);
        memcpy(data.data() + Offsets::version,    header.version.constData(), Sizes::version);
        memcpy(data.data() + Offsets::salt,       header.salt.constData(),    Sizes::salt);
        memcpy(data.data() + Offsets::iteration,  &header.iteration,          Sizes::iteration);
        memcpy(data.data() + Offsets::hmac,       header.hmac.constData(),    Sizes::hmac);
        memcpy(data.data() + Offsets::iv,         header.iv.constData(),      Sizes::iv);
        return data;
    }

public:
    enum State{
        PlainData,
        CipherData
    };

    bool integrity = true;
    bool isHeaderMatch = true;
    State state = PlainData;
    FileHeader header;
    FilePath path;
};
