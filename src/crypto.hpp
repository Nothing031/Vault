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
#include <QQueue>

#include "vault.h"
#include "file_t.hpp"

using namespace std;
namespace fs = std::filesystem;

enum CRYPTIONMODE{
    ENCRYPTION,
    DECRYPTION
};



class Crypto : public QObject{
    Q_OBJECT
    class Error{
    public:
        enum eError{
            Unknown,
            FileNotExists,
            FailedToRead,
            FailedToSave,
            Encryption,
            Decryption,

        };
        Error() : e_type(Unknown), e_what("Unknown Error"){

        }
        Error(const eError type, const QString what) : e_type(type), e_what(what){

        }
    private:
        eError e_type;
        QString e_what;

        eError& type(){
            return e_type;
        }
        QString& what(){
            return e_what;
        };
    };

private:
    bool state_running = false;

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

    static void AES256_Encrypt(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        // init
        if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
            throw Error(Error::Decryption, "Failed to finalize EVP");
        }
        int blockSize =EVP_CIPHER_CTX_get_block_size(ctx);
        out.resize(input.size() + blockSize);
        // encrypt
        int len;
        if (1 != EVP_EncryptUpdate(ctx, (unsigned char*)out.data(), &len, (const unsigned char*)input.data(), input.size())) {
            throw Error(Error::Decryption, "Failed to encrypt");
        }
        // finalize
        int outLen = len;
        if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char*)out.data() + len, &len)) {
            throw Error(Error::Decryption, "Failed to finalize EVP");
        }
        outLen += len;
        out.resize(outLen);
        EVP_CIPHER_CTX_free(ctx);
    }
    static void AES256_Decrypt(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

        // init
        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
            throw Error(Error::Decryption, "Failed to initialize EVP");
        }
        out.resize(input.size(), 0);
        // decrypt
        int len;
        if (1 != EVP_DecryptUpdate(ctx, (unsigned char*)out.data(), &len, (const unsigned char*)input.constData(), input.size())) {
            throw Error(Error::Decryption, "Failed to decrypt");
        }
        // finalize
        int outLen = len;
        if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char*)out.data() + len, &len)) {
            throw Error(Error::Decryption, "Failed to finalize EVP");
        }
        outLen += len;
        out.resize(outLen);
        EVP_CIPHER_CTX_free(ctx);
    }

    inline static void AES256_Encrypt_File(file_t* file, const QByteArray& key){
        QByteArray plainData;
        if (!ReadFile(file->absolutepath, plainData)){
            throw Error(Error::FailedToRead, "Failed to read : " + file->absolutepath);
        }
        QByteArray iv;
        RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), 16);

        QByteArray cipherData;
        AES256_Encrypt(plainData, key, iv, cipherData);

        cipherData.insert(0, iv);
        if (WriteFile(file->absolutepath + ".enc", cipherData)){
            throw Error(Error::FailedToSave, "Failed to save to: " + file->absolutepath);
        }
        file->absolutepath+= ".enc";
        file->relativePath+= ".enc";
        file->state = file_t_state::CipherData;
    }
    inline static void AES256_Decrypt_File(file_t* file, const QByteArray& key){
        QByteArray cipherData;
        if (!ReadFile(file->absolutepath, cipherData)){
            throw Error(Error::FailedToRead, "Failed to read : " + file->absolutepath);
        }
        QByteArray iv = cipherData.mid(0, 16);
        cipherData.erase(cipherData.begin(), cipherData.begin() + 16);

        QByteArray plainData;
        AES256_Decrypt(cipherData, key, iv, plainData);


        if (WriteFile(file->absolutepath.left(file->absolutepath.size() - 4), plainData)){
            throw Error(Error::FailedToSave, "Failed to save to: " + file->absolutepath);
        }
        file->absolutepath = file->absolutepath.left(file->absolutepath.size() - 4);
        file->relativePath = file->relativePath.left(file->relativePath.size() - 4);
        file->state = file_t_state::PlainData;
    }

    void AES256_Encrypt_All(Vault& vault){
        emit signal_start();
        vault.mutex.lock();

        int threadCount = std::thread::hardware_concurrency();
        QVector<QThread*> threads(threadCount);

        QQueue<file_t*> files;
        QMutex files_m;
        for (int i = 0; i < vault.files.size(); i++){
            if (vault.files[i].state == file_t_state::PlainData){
                files.push_back(&vault.files[i]);
            }
        }

        for (int i = 0; i < threadCount; i++){
            threads[i] = QThread::create([&](){
                while(true){
                    files_m.lock();
                    if (files.empty()){
                        files_m.unlock();
                        return;
                    }
                    file_t* file = files.front();
                    files.pop_front();
                    files_m.unlock();

                    qDebug() << file->displayPath;
                    //AES256_Encrypt_File(file, vault.key);
                }
            });
            threads[i]->start();
        }

        for (int i = 0; i < threadCount; i++){
            if (threads[i]->isRunning()){
                threads[i]->wait();
            }
        }

        vault.mutex.unlock();
        emit signal_done();
    }
    void AES256_Decrypt_All(Vault& vault){
        emit signal_start();
        vault.mutex.lock();

        int threadCount = std::thread::hardware_concurrency();
        QVector<QThread*> threads(threadCount);

        QQueue<file_t*> files;
        QMutex files_m;
        for (int i = 0; i < vault.files.size(); i++){
            if (vault.files[i].state == file_t_state::CipherData){
                files.push_back(&vault.files[i]);
            }
        }

        // start
        for (int i = 0; i < threadCount; i++){
            threads[i] = QThread::create([&](){
                while(true){
                    files_m.lock();
                    if (files.empty()){
                        files_m.unlock();
                        return;
                    }
                    file_t* file = files.front();
                    files.pop_front();
                    files_m.unlock();
                    qDebug() << file->displayPath;

                    //AES256_Decrypt_File(file, vault.key);
                }
            });
            threads[i]->start();
        }

        // wait
        for (int i = 0; i < threadCount; i++){
            if (threads[i]->isRunning()){
                threads[i]->wait();
            }
        }

        vault.mutex.unlock();
        emit signal_done();
    }

public:
    bool flag_run = false;
    inline static QString SHA256(const QString& str)
    {
        return QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Algorithm::Sha256).toHex();
    }

signals:
    void signal_outputMessage(QStringList messages);
    void signal_start();
    void signal_progress(int value);
    void signal_done();
};


#endif // CRYPTO_HPP
