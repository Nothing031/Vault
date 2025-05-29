#pragma once

#include <QByteArray>
#include <QString>

#define FORMAT_VERSION "0002"
#define SIGNATURE "VAULT BY NOTHING031"
#define ITERATION 100000
#define EXTENSION ".enc"

#include "FileInfo.hpp"
#include "src/core/vault/AES256Settings.hpp"

struct FileHeader{
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
private:
    inline static const QByteArray currentVersion = []{QByteArray data(Sizes::version, 0); memcpy(data.data(), FORMAT_VERSION, Sizes::version); return data;}();
public:
    inline static const QByteArray signature = []{QByteArray data(Sizes::signature, 0); memcpy(data.data(), SIGNATURE, strlen(SIGNATURE)); return data;}();

    QByteArray version = currentVersion;
    QByteArray salt = {};
    int iteration = ITERATION;
    QByteArray hmac = {};
    QByteArray iv = {};

    void RandIv();
    FileInfo::State Deserialize(const QByteArray& data);
    FileInfo::State Deserialize(const QByteArray& data, const QByteArray& hmac);
    QByteArray Serialize() const;
    void SetData(const AES256Settings& aes);
};



