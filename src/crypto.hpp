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
    QThread *thread;

    bool flag_run;
    bool state_running;
\
    bool AES256_Encrypt_File(const QString& inPath, const QString& outPath, const QByteArray& key)
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
    bool AES256_Decrypt_File(const QString& inPath, const QString& outPath, const QByteArray& key)
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
    void AES256_EN_DE_Crypt_All(Vault& vault, const QByteArray key, const CRYPTIONMODE cryptionMode, const int processor_count)
    {
        emit signal_start();
        QVector<FILE_INFO*> targetFiles;
        QVector<FILE_INFO*>::Iterator file_info_it = targetFiles.begin();

        for (FILE_INFO& file : vault.files){
            if (cryptionMode == ENCRYPTION && !file.encrypted){
                targetFiles.push_back(&file);
            }else if (cryptionMode == DECRYPTION && file.encrypted){
                targetFiles.push_back(&file);
            }
        }

        int thread_count = static_cast<int>(targetFiles.size() > processor_count ? processor_count : targetFiles.size());
        int files_per_thread = static_cast<int>(targetFiles.size() / thread_count);
        int remaining = targetFiles.size() % thread_count;

        std::unique_ptr<QThread*[]> pWorkerArr = std::make_unique<QThread*[]>(thread_count);
        std::unique_ptr<QVector<FILE_INFO*>[]> pFilesArr = std::make_unique<QVector<FILE_INFO*>[]>(thread_count);

        for (int tI = 0; tI < thread_count; tI++){
            int files_for_current = files_per_thread + (tI < remaining ? 1 : 0);
            for (int j = 0; j < files_for_current; j++){
                pFilesArr[tI].push_back(*file_info_it);
                file_info_it++;
            }
        }

        // start
        QMutex mutex;
        QStringList successList;
        QStringList failList;

        QStringList flushList;

        QElapsedTimer timer;
        timer.start();
        qDebug() << "[WORKER] Starting process";
        qDebug() << "  Cryption mode : " << (cryptionMode == ENCRYPTION ? "Encryption" : "Decryption");
        qDebug() << "  Files         : " << targetFiles.size();
        for (int tI = 0; tI < thread_count; tI++){
            pWorkerArr[tI] = QThread::create([tI, cryptionMode, &mutex, &successList, &failList, &flushList, &pFilesArr, key, this](){
                qDebug() << "[WORKER] Thread " << tI << " started";
                for (int i = 0; i < pFilesArr[tI].size(); i++){
                    // check flag
                    if (flag_run){
                        qDebug() << "[WORKER] Thread " << tI << " Suspended";
                        break;
                    }
                    // en / decrypt
                    if (cryptionMode == ENCRYPTION){
                        if (!pFilesArr[tI][i]->file.exists()){
                            mutex.lock();
                            qDebug() << "[WORKER] Thread " << tI << ": error file not exists " << pFilesArr[tI][i]->file.path();
                            flushList.push_back("Error file not exists : " + pFilesArr[tI][i]->file.path());
                            failList.push_back("Error file not exists : " + pFilesArr[tI][i]->file.path());
                            mutex.unlock();
                        }
                        QString inPath = pFilesArr[tI][i]->file.path();
                        QString outPath = inPath + ".enc";
                        if (AES256_Encrypt_File(inPath, outPath, key)){
                            pFilesArr[tI][i]->encrypted = true;
                            pFilesArr[tI][i]->file = QFileInfo(outPath);
                            mutex.lock();
                            successList.push_back("File encrypted : " + pFilesArr[tI][i]->file.path());
                            flushList.push_back("File encrypted : " + pFilesArr[tI][i]->file.path());
                            mutex.unlock();
                            try{
                                if (QFile(inPath).exists()) QFile::remove(inPath);
                            }catch(...){
                                qDebug() << "[WORKER] Thread " << tI << ": error failed to remove file " << inPath;
                                mutex.lock();
                                flushList.push_back("Error failed to remove file : " + inPath);
                                mutex.unlock();
                            }
                        }
                        else{
                            mutex.lock();
                            failList.push_back("Error failed to encrypt : " + pFilesArr[tI][i]->file.path());
                            flushList.push_back("Error failed to encrypt : " + pFilesArr[tI][i]->file.path());
                            mutex.unlock();
                            try{
                                if (QFile(outPath).exists()) QFile::remove(outPath);
                            }catch(...){
                                qDebug() << "[WORKER] Thread " << tI << ": error failed to remove file " << outPath;
                                mutex.lock();
                                flushList.push_back("Error failed to remove file : " + outPath);
                                mutex.unlock();
                            }
                        }
                    }else{
                        if (!pFilesArr[tI][i]->file.exists()){
                            mutex.lock();
                            qDebug() << "[WORKER] Thread " << tI << ": error file not exists " << pFilesArr[tI][i]->file.path();
                            flushList.push_back("Error file not exists : " + pFilesArr[tI][i]->file.path());
                            failList.push_back("Error file not exists : " + pFilesArr[tI][i]->file.path());
                            mutex.unlock();
                        }
                        QString inPath = pFilesArr[tI][i]->file.path();
                        QString outPath = inPath.left(inPath.size() - 4);
                        if (AES256_Decrypt_File(inPath, outPath, key)){
                            pFilesArr[tI][i]->encrypted = false;
                            pFilesArr[tI][i]->file = QFileInfo(outPath);
                            mutex.lock();
                            successList.push_back("File decrypted : " + pFilesArr[tI][i]->file.path());
                            flushList.push_back("File decrypted : " + pFilesArr[tI][i]->file.path());
                            mutex.unlock();
                            try{
                                if (QFile(inPath).exists()) QFile::remove(inPath);
                            }catch(...){
                                qDebug() << "[WORKER] Thread " << tI << ": error failed to remove file " << inPath;
                                mutex.lock();
                                flushList.push_back("Error failed to remove file : " + inPath);
                                mutex.unlock();
                            }
                        }
                        else{
                            mutex.lock();
                            failList.push_back("Error failed to decrypt : " + pFilesArr[tI][i]->file.path());
                            flushList.push_back("Error failed to decrypt : " + pFilesArr[tI][i]->file.path());
                            mutex.unlock();
                            try{
                                if (QFile(outPath).exists()) QFile::remove(outPath);
                            }catch(...){
                                qDebug() << "[WORKER] Thread " << tI << ": error failed to remove file " << outPath;
                                mutex.lock();
                                flushList.push_back("Error failed to remove file : " + outPath);
                                mutex.unlock();
                            }
                        }
                    }
                }
                qDebug() << "[WORKER] Thread " << tI << " done";
            });
        }

        // signal thread;
        bool flag_run_signalSender = true;
        QThread *signalSender = QThread::create([&flag_run_signalSender, &mutex, &successList, &failList , &flushList, this](){
            while (flag_run_signalSender){
                mutex.lock();
                emit signal_progress(successList.size() + failList.size());
                emit signal_outputMessage(flushList);
                flushList.clear();
                mutex.unlock();
                QThread::msleep(500);
            }
        });

        // wait
        for (int tI = 0; tI < thread_count; tI++){
            if (!pWorkerArr[tI]->isFinished())
                pWorkerArr[tI]->wait();
        }
        // stop signalSender
        flag_run_signalSender = false;

        // wait
        if (!signalSender->isFinished())
            signalSender->wait();

        flag_run = false;
        state_running = false;
        qDebug() << "[WORKER] Processing done";
        qDebug() << "  Elapsed time  : " << timer.elapsed() << "ms";
        qDebug() << "  Successed     : " << successList.size();
        qDebug() << "  Failed        : " << failList.size();

        emit signal_outputMessage(flushList);
        emit signal_done();
        flushList.clear();
        flushList = {"Done",
                    "  Elapsed time : " + QString::number(timer.elapsed()) + "ms",
                    "  Successed    : " + QString::number(successList.size()),
                    "  Failed      : " + QString::number(failList.size())
                    };
        emit signal_outputMessage(flushList);
    }

public:
    static bool ReadFile(const QString& path, QByteArray& out)
    {
        out.clear();
        QFile f(path);
        if (f.open(QFile::ReadOnly)){
            out = f.readAll();
            return true;
        }else{
            return false;
        }
    }
    static bool WriteFile(const QString& path, const QByteArray& bytes)
    {
        QFile f(path);
        if (!f.open(QFile::WriteOnly)){
            f.write(bytes);
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
        QByteArray encryptedData;

        // init
        if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
            qDebug() << "[ENCRYPT] EVP_EncryptInit_ex Failure";
            return false;
        }
        // encrypt
        int len;
        if (1 != EVP_EncryptUpdate(ctx, (unsigned char*)encryptedData.data(), &len, (const unsigned char*)input.data(), input.size())) {
            qDebug() << "[ENCRYPT] EVP_EncryptUpdate Failure";
            return false;
        }
        // finalize
        int encryptedLen = len;
        if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char*)encryptedData.data() + len, &len)) {
            qDebug() << "[ENCRYPT] EVP_EncryptFinal_ex Failure";
            return false;
        }
        encryptedLen += len;
        encryptedData.resize(encryptedLen);
        out = encryptedData;
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }
    static bool AES256_Decrypt(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        QByteArray decryptedData;

        // init
        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.constData(), (const unsigned char*)iv.constData())) {
            qDebug() << "[DECRYPT] EVP_DecryptInit_ex Failure";
            return false;
        }
        // decrypt
        int len;
        if (1 != EVP_DecryptUpdate(ctx, (unsigned char*)decryptedData.data(), &len, (const unsigned char*)input.constData(), input.size())) {
            qDebug() << "[DECRYPT] EVP_DecryptUpdate Failure";
            return false;
        }
        // finalize
        int decryptedLen = len;
        if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char*)decryptedData.data() + len, &len)) {
            qDebug() << "[DECRYPT] EVP_DecryptFinal_ex Failure";
            return false;
        }
        decryptedLen += len;
        decryptedData.resize(decryptedLen);
        out = decryptedData;
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }

    bool start_encrypt(Vault& vault, const QByteArray key)
    {
        if (state_running){
            return false;
        }
        thread = QThread::create([&vault, key, this](){
            flag_run = true;
            state_running = true;
            AES256_EN_DE_Crypt_All(vault, key, ENCRYPTION, std::thread::hardware_concurrency());
        });
        thread->start();
        return true;
    }
    bool start_decrypt(Vault& vault, const QByteArray key)
    {
        if (state_running){
            return false;
        }
        thread = QThread::create([&vault, key, this](){
            flag_run = true;
            state_running = true;
            AES256_EN_DE_Crypt_All(vault, key, DECRYPTION, std::thread::hardware_concurrency());
        });
        thread->start();
        return true;
    }
    void suspend()
    {
        flag_run = false;
    }

public slots:


signals:
    void signal_outputMessage(QStringList messages);
    void signal_start();
    void signal_progress(int value);
    void signal_done();
};


#endif // CRYPTO_HPP
