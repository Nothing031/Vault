#pragma once

#include <QObject>
#include <QCryptographicHash>
#include <QElapsedTimer>
#include <QMutex>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QByteArray>
#include <QQueue>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "Error.hpp"

namespace Cryptography
{
inline static void AES_256_Encrypt(const QByteArray& in, const QByteArray& key, const QByteArray& iv, QByteArray& out, Error& error){
    // init
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (1 != EVP_EncryptInit_ex(ctx,
                                EVP_aes_256_cbc(),
                                NULL,
                                (const unsigned char*)key.constData(),
                                (const unsigned char*)iv.constData())){
        EVP_CIPHER_CTX_free(ctx);
        error.Set(Error::CRYPTO_EVP_INIT_FAILURE, "Failed to initialize EVP", "");
        return;
    }

    // encrypt
    out.resize(in.size() + AES_BLOCK_SIZE);
    int outLen;
    if (1 != EVP_EncryptUpdate(ctx,
                               (unsigned char*)out.data(),
                               &outLen,
                               (const unsigned char*)in.data(),
                               in.size())) {
        EVP_CIPHER_CTX_free(ctx);
        error.Set(Error::CRYPTO_EVP_UPDATE_FAILURE, "Failed to encrypt", "");
        return;
    }
    int finalLen = outLen;


    // finalize
    if (1 != EVP_EncryptFinal_ex(ctx,
                                 (unsigned char*)out.data() + outLen,
                                 &outLen)) {
        EVP_CIPHER_CTX_free(ctx);
        error.Set(Error::CRYPTO_EVP_FINALIZE_FAILURE, "Failed to finalize EVP", "");
        return;
    }
    finalLen += outLen;
    out.resize(finalLen);
    EVP_CIPHER_CTX_free(ctx);
    return;
}
inline static void AES_256_Decrypt(const QByteArray& in, const QByteArray& key, const QByteArray& iv, QByteArray& out, Error& error){
    // init
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (1 != EVP_DecryptInit_ex(ctx,
                                EVP_aes_256_cbc(),
                                NULL,
                                (const unsigned char*)key.constData(),
                                (const unsigned char*)iv.constData())) {
        EVP_CIPHER_CTX_free(ctx);
        error.Set(Error::CRYPTO_EVP_INIT_FAILURE, "Failed to initialize EVP", "");
        return;
    }
    out.resize(in.size());

    // decrypt
    int outLen;
    if (1 != EVP_DecryptUpdate(ctx,
                               (unsigned char*)out.data(),
                               &outLen,
                               (const unsigned char*)in.constData(),
                               in.size())) {
        EVP_CIPHER_CTX_free(ctx);
        error.Set(Error::CRYPTO_EVP_UPDATE_FAILURE, "Failed to decrypt", "");
        return;
    }

    // finalize
    int finalLen = outLen;
    if (1 != EVP_DecryptFinal_ex(ctx,
                                 (unsigned char*)out.data() + outLen,
                                 &outLen)) {
        EVP_CIPHER_CTX_free(ctx);
        error.Set(Error::CRYPTO_EVP_FINALIZE_FAILURE, "Failed to finalize EVP", "");
        return;
    }
    finalLen += outLen;
    out.resize(finalLen);
    EVP_CIPHER_CTX_free(ctx);
    return;
}
inline static QByteArray GenerateKey(const QString str, const QByteArray& salt, const int itr){
    QByteArray key(64, 0);
    int result = PKCS5_PBKDF2_HMAC((const char*)str.constData(),
                                   str.size(),
                                   (const unsigned char*)salt.constData(),
                                   salt.size(),
                                   itr,
                                   EVP_sha256(),
                                   64,
                                   (unsigned char*)key.data()
                                   );
    if (result == 0){
        qWarning() << "Failed to generate key";
        throw std::runtime_error("Failed to generate key");
    }
    return key;
}


};



