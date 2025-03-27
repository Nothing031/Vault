#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <QObject>
#include <QCryptographicHash>
#include <QElapsedTimer>
#include <QMutex>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QByteArray>
#include <QQueue>

#include <thread>
#include <ctime>
#include <atomic>

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "vault.hpp"
#include "file_t.hpp"
#include "utils.hpp"


enum CRYPTIONMODE{
    ENCRYPTION,
    DECRYPTION
};

class Crypto : public QObject{
    Q_OBJECT
public:
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
        Error(const eError type = eError::Unknown, const QString what = "Unknown Error") : e_type(type), e_what(what){

        }

        eError& type(){
            return e_type;
        }
        QString& what(){
            return e_what;
        };
    private:
        eError e_type;
        QString e_what;
    };

private:
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
        if (f.open(QFile::WriteOnly)){
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

        // encrypt
        out.resize(input.size() + AES_BLOCK_SIZE);
        int outLen;
        if (1 != EVP_EncryptUpdate(ctx, (unsigned char*)out.data(), &outLen, (const unsigned char*)input.data(), input.size())) {
            throw Error(Error::Decryption, "Failed to encrypt");
        }
        int finalLen = outLen;
        // finalize
        if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char*)out.data() + outLen, &outLen)) {
            throw Error(Error::Decryption, "Failed to finalize EVP");
        }
        finalLen += outLen;
        out.resize(finalLen);
        EVP_CIPHER_CTX_free(ctx);
    }
    static void AES256_Decrypt(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

        // init
        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
            EVP_CIPHER_CTX_free(ctx);
            throw Error(Error::Decryption, "Failed to initialize EVP");
        }
        out.resize(input.size());
        // decrypt
        int outLen;
        if (1 != EVP_DecryptUpdate(ctx, (unsigned char*)out.data(), &outLen, (const unsigned char*)input.constData(), input.size())) {
            EVP_CIPHER_CTX_free(ctx);
            throw Error(Error::Decryption, "Failed to decrypt");
        }
        // finalize
        int finalLen = outLen;
        if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char*)out.data() + outLen, &outLen)) {
            EVP_CIPHER_CTX_free(ctx);
            throw Error(Error::Decryption, "Failed to finalize EVP");
        }
        finalLen += outLen;
        out.resize(finalLen);
        EVP_CIPHER_CTX_free(ctx);
    }

public:
    static void AES256_Encrypt_File(file_t* file, const QByteArray& key){
        QByteArray plainData;
        if (!ReadFile(file->absolutepath, plainData)){
            throw Error(Error::FailedToRead, "Failed to read : " + file->absolutepath);
        }
        QByteArray iv;
        iv.resize(16);
        RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), 16);

        QByteArray cipherData;
        AES256_Encrypt(plainData, key, iv, cipherData);

        cipherData.insert(0, iv);
        if (!WriteFile(file->absolutepath + ".enc", cipherData)){
            throw Error(Error::FailedToSave, "Failed to save to: " + file->absolutepath);
        }
        QFile::remove(file->absolutepath);

        file->absolutepath+= ".enc";
        file->relativePath+= ".enc";
        file->state = file_t::CipherData;
    }
    static void AES256_Decrypt_File(file_t* file, const QByteArray& key){
        QByteArray cipherData;
        if (!ReadFile(file->absolutepath, cipherData)){
            throw Error(Error::FailedToRead, "Failed to read : " + file->absolutepath);
        }
        QByteArray iv = cipherData.mid(0, 16);
        cipherData.erase(cipherData.begin(), cipherData.begin() + 16);

        QByteArray plainData;
        AES256_Decrypt(cipherData, key, iv, plainData);

        if (!WriteFile(file->absolutepath.left(file->absolutepath.size() - 4), plainData)){
            throw Error(Error::FailedToSave, "Failed to save to: " + file->absolutepath);
        }
        QFile::remove(file->absolutepath);
        file->absolutepath = file->absolutepath.left(file->absolutepath.size() - 4);
        file->relativePath = file->relativePath.left(file->relativePath.size() - 4);
        file->state = file_t::PlainData;
    }

    void AES256_Encrypt_All(Vault* vault){
        emit signal_start();
        flag_suspend = false;

        int processorCount = std::thread::hardware_concurrency();
        int threadCount = vault->plainIndex.size() < processorCount ? vault->plainIndex.size() : processorCount;
        QVector<QThread*> threads(threadCount);

        std::atomic<int> success = 0;
        std::atomic<int> failed = 0;

        QStringList flushMessages;
        QStringList errorMessages;

        QMutex messages_m;
        QQueue<file_t*> files;
        QMutex files_m;
        for (int i = 0; i < vault->files.size(); i++){
            if (vault->files[i].state == file_t::PlainData){
                files.push_back(&vault->files[i]);
            }
        }

        // work
        for (int i = 0; i < threadCount; i++){
            threads[i] = QThread::create([&](){
                while(true){
                    if (flag_suspend){
                        qDebug()  << "!!!!!!!!!!!!!!! suspending !!!!!!!!!!!!!!";
                        return;
                    }

                    files_m.lock();
                    if (files.empty()){
                        files_m.unlock();
                        return;
                    }
                    file_t* file = files.front();
                    files.pop_front();
                    files_m.unlock();

                    try{
                        QThread::msleep(500);
                        qDebug() << file->displayPath;
                        //AES256_Encrypt_File(file, vault.key);
                        success++;
                    }catch(Error& e){
                        messages_m.lock();
                        flushMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
                        flushMessages.append("<font color='red'>  \"" + e.what() + "\"</font>");
                        errorMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
                        errorMessages.append("<font color='red'>  \"" + e.what() + "\"</font>");
                        messages_m.unlock();
                        failed++;
                    }catch(std::exception& e){
                        messages_m.lock();
                        flushMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
                        flushMessages.append("<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"</font>");
                        errorMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
                        errorMessages.append("<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"</font>");
                        messages_m.unlock();
                        failed++;
                    }
                }
            });
            threads[i]->start();
        }

        // output
        while (true){
            Utils::Sleep(500, 16, !flag_suspend);
            files_m.lock();
            if (files.empty()){
                files_m.unlock();
                emit signal_terminal_message(flushMessages);
                emit signal_progress(success + failed);
                break;
            }
            if  (flag_suspend){
                files_m.unlock();
                break;
            }
            files_m.unlock();

            messages_m.lock();
            emit signal_terminal_message(flushMessages);
            flushMessages.clear();
            messages_m.unlock();
            emit signal_progress(success + failed);
        }

        // wait
        for (int i = 0; i < threadCount; i++){
            if (threads[i]->isRunning()){
                threads[i]->wait();
            }
        }

        flag_suspend = false;
        flushMessages.clear();
        emit signal_terminal_clear();
        if (success){
            flushMessages.append("<font  color='green'>" + QString::number(success) + "Files encrypted</font>");
        }
        if (failed){
            flushMessages.append("<font  color='green'>" + QString::number(success) + "Files failed to encrypt</font>");
        }
        emit signal_terminal_message(flushMessages);
        if (failed){
            emit signal_terminal_message(errorMessages);
        }
        emit signal_done();
    }
    void AES256_Decrypt_All(Vault* vault){
        emit signal_start();
        flag_suspend = true;

        int processorCount = std::thread::hardware_concurrency();
        int threadCount = vault->cipherIndex.size() < processorCount ? vault->cipherIndex.size() : processorCount;
        QVector<QThread*> threads(threadCount);

        std::atomic<int> progress = 0;
        QStringList messages;
        QMutex messages_m;
        QQueue<file_t*> files;
        QMutex files_m;
        for (int i = 0; i < vault->files.size(); i++){
            if (vault->files[i].state == file_t::CipherData){
                files.push_back(&vault->files[i]);
            }
        }

        // work
        for (int i = 0; i < threadCount; i++){
            threads[i] = QThread::create([&](){
                while(flag_suspend){
                    files_m.lock();
                    if (files.empty()){
                        files_m.unlock();
                        return;
                    }
                    file_t* file = files.front();
                    files.pop_front();
                    files_m.unlock();

                    try{
                        QThread::msleep(500);
                        qDebug() << file->displayPath;
                        //AES256_Decrypt_File(file, vault.key);
                        messages_m.lock();
                        messages.append("<font color='green'>\"" + file->absolutepath + "\" decrypted</font>");
                        messages_m.unlock();
                    }catch(Error& e){
                        messages_m.lock();
                        messages.append("<font color='red'>\"" + file->absolutepath + "\" failed to decrypt</font>");
                        messages.append("<font color='red'>  \"" + e.what() + "\"");
                        messages_m.unlock();
                    }catch(std::exception& e){
                        messages_m.lock();
                        messages.append("<font color='red'>\"" + file->absolutepath + "\" failed to decrypt</font>");
                        messages.append("<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"");
                        messages_m.unlock();
                    }

                    progress++;
                }
            });
            threads[i]->start();
        }

        // output
        while (flag_suspend){
            Utils::Sleep(1000, 16, flag_suspend);
            files_m.lock();
            if (files.empty()){
                files_m.unlock();
                break;
            }
            files_m.unlock();
            messages_m.lock();
            emit signal_terminal_message(messages);
            messages.clear();
            messages_m.unlock();
            emit signal_progress(progress);
        }

        // wait
        for (int i = 0; i < threadCount; i++){
            if (threads[i]->isRunning()){
                threads[i]->wait();
            }
        }

        flag_suspend = false;
        emit signal_done();
    }

    bool flag_suspend = false;
    inline static QString SHA256(const QString& str)
    {
        return QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Algorithm::Sha256).toHex();
    }


signals:
    void signal_terminal_clear();
    void signal_terminal_message(QStringList messages);
    void signal_progress(int value);
    void signal_start();
    void signal_done();
};


#endif // CRYPTO_HPP
