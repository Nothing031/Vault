#ifndef CRYPTO_HPP
#define CRYPTO_HPP


#include <thread>
#include <ctime>
#include <memory>
#include <QObject>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include <QCryptographicHash>
#include <QElapsedTimer>
#include <QMutex>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QByteArray>

#include "vault.h"

using namespace std;
namespace fs = std::filesystem;

enum CRYPTIONMODE{
    ENCRYPTION,
    DECRYPTION
};


class Crypto : public QObject{
    Q_OBJECT

private:
    bool state_running = false;

    inline static bool AES256_Encrypt_File(const QString& inPath, const QString& outPath, const QByteArray& key)
    {
        QByteArray plainData;
        if (!ReadFile(inPath, plainData)) {
            qDebug() << "[ENCRYPT] Failed to read file, path : " << inPath;
            return false;
        }
        // create iv
        QByteArray iv;
        RAND_bytes((unsigned char*)iv.data(), 16);
        // encrypt
        QByteArray encryptedData;
        if (!AES256_Encrypt(plainData, key, iv, encryptedData)) {
            qDebug() << "[ENCRYPT] Failed to encrypt file, path : " << inPath;
            return false;
        }
        // insert iv
        encryptedData.insert(0, iv);
        if (!WriteFile(outPath, encryptedData)){
            qDebug() << "[ENCRYPT] Failed to save file to " << outPath;
            return false;
        }
        return true;
    }
    inline static bool AES256_Decrypt_File(const QString& inPath, const QString& outPath, const QByteArray& key)
    {
        QByteArray cipherData;
        if (!ReadFile(inPath, cipherData)){
            qDebug() << "[DECRYPT] Failed to read file, path : " << inPath;
            return false;
        }
        // read iv
        QByteArray iv = cipherData.mid(0, 16);
        // erase iv
        cipherData.erase(cipherData.begin(), cipherData.begin() + 16);
        // decrypt
        QByteArray plainData;
        if (AES256_Decrypt(cipherData, key, iv, plainData)){
            qDebug() << "[DECRYPT] Failed to decrypt file, path : " << inPath;
            return false;
        }
        if (WriteFile(outPath, plainData)){
            qDebug() << "[DECRYPT] Failed to save file to " << outPath;
            return false;
        }
        return true;
    }
public:
    bool flag_run = false;

    inline static bool ReadFile(const QString& path, QByteArray& out)
    {
        out.clear();
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)){
            out.resize(f.size());
            out = f.readAll();
            f.close();
            return true;
        }else{
            return false;
        }
    }
    inline static bool WriteFile(const QString& path, const QByteArray& bytes)
    {
        QFile f(path);
        if (f.open(QIODevice::WriteOnly)){
            f.write(bytes);
            f.close();
            return true;
        }else{
            return false;
        }
    }

    inline static QString SHA256(const QString& str)
    {
        return QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Algorithm::Sha256).toHex();
    }

    static bool AES256_Encrypt(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        // init
        if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
            qDebug() << "[ENCRYPT] EVP_EncryptInit_ex Failure";
            return false;
        }
        int blockSize =EVP_CIPHER_CTX_get_block_size(ctx);
        out.resize(input.size() + blockSize);
        // encrypt
        int len;
        if (1 != EVP_EncryptUpdate(ctx, (unsigned char*)out.data(), &len, (const unsigned char*)input.data(), input.size())) {
            qDebug() << "[ENCRYPT] EVP_EncryptUpdate Failure";
            return false;
        }
        // finalize
        int outLen = len;
        if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char*)out.data() + len, &len)) {
            qDebug() << "[ENCRYPT] EVP_EncryptFinal_ex Failure";
            return false;
        }
        outLen += len;
        out.resize(outLen);
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }
    static bool AES256_Decrypt(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

        // init
        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
            qDebug() << "[DECRYPT] EVP_DecryptInit_ex Failure";
            return false;
        }
        out.resize(input.size(), 0);
        // decrypt
        int len;
        if (1 != EVP_DecryptUpdate(ctx, (unsigned char*)out.data(), &len, (const unsigned char*)input.constData(), input.size())) {
            qDebug() << "[DECRYPT] EVP_DecryptUpdate Failure";
            return false;
        }
        // finalize
        int outLen = len;
        if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char*)out.data() + len, &len)) {
            qDebug() << "[DECRYPT] EVP_DecryptFinal_ex Failure";
            return false;
        }
        outLen += len;
        out.resize(outLen);
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }

public slots:
    void AES256_Encrypt_All(Vault& vault){
        emit signal_start();
        vault.mutex.lock();
        QVector<FILE_INFO*> targetFiles;
        for (const auto& index : vault.index_decrypted){
            targetFiles.push_back(&vault.files[index]);
        }
        int processor_count = std::thread::hardware_concurrency();
        int thread_count = static_cast<int>(targetFiles.size() > processor_count ? processor_count : targetFiles.size());
        int files_per_thread = static_cast<int>(targetFiles.size() / thread_count);
        int remaining = targetFiles.size() % thread_count;

        std::unique_ptr<QThread*[]> pWorkerArr = std::make_unique<QThread*[]>(thread_count);
        std::unique_ptr<QVector<FILE_INFO*>[]> pFilesArr = std::make_unique<QVector<FILE_INFO*>[]>(thread_count);
        int file_info_it = 0;

        for (int tI = 0; tI < thread_count; tI++){
            int files_for_current = files_per_thread + (tI < remaining ? 1 : 0);
            for (int j = 0; j < files_for_current; j++){
                qDebug() << targetFiles[file_info_it]->relativePath;
                pFilesArr[tI].push_back(targetFiles[file_info_it++]);
            }
        }


        vault.mutex.unlock();
        emit signal_done();
    }
    void AES256_Decrypt_All(Vault& vault){
        QMetaObject::invokeMethod(this, [this](){
            emit signal_start();
        });
        vault.mutex.lock();
        QVector<FILE_INFO*> targetFiles;
        for (const auto& index : vault.index_encrypted){
            targetFiles.push_back(&vault.files[index]);
        }
        int processor_count = std::thread::hardware_concurrency();
        int thread_count = static_cast<int>(targetFiles.size() > processor_count ? processor_count : targetFiles.size());
        int files_per_thread = static_cast<int>(targetFiles.size() / thread_count);
        int remaining = targetFiles.size() % thread_count;

        std::unique_ptr<QThread*[]> pWorkerArr = std::make_unique<QThread*[]>(thread_count);
        std::unique_ptr<QVector<FILE_INFO*>[]> pFilesArr = std::make_unique<QVector<FILE_INFO*>[]>(thread_count);
        int file_info_it = 0;

        for (int tI = 0; tI < thread_count; tI++){
            int files_for_current = files_per_thread + (tI < remaining ? 1 : 0);
            for (int j = 0; j < files_for_current; j++){
                qDebug() << targetFiles[file_info_it]->relativePath;
                pFilesArr[tI].push_back(targetFiles[file_info_it++]);
            }
        }


        vault.mutex.unlock();
        QMetaObject::invokeMethod(this, [this](){
            emit signal_done();
        });
    }

signals:
    void signal_outputMessage(QStringList messages);
    void signal_start();
    void signal_progress(int value);
    void signal_done();
};


#endif // CRYPTO_HPP
