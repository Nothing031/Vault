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

#include <ctime>
#include <atomic>

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "vault.hpp"
#include "filemetadata.hpp"
#include "utils.hpp"
#include "enviroment.hpp"


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
            HeaderNotMatch,
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

    Crypto(){

    }
    ~Crypto(){
        if (thread){
            delete thread;
        }
    }

private:
    void AES256_Encrypt_All(Vault* vault)
    {
        qDebug() << "empty function";
        // emit signal_start();
        // flag_suspend = false;

        // // --- progress ---
        // std::atomic<int> success = 0;
        // std::atomic<int> failed = 0;

        // // --- message ---
        // QStringList flushMessages;
        // QStringList errorMessages;
        // QMutex messages_m;

        // // --- files ---
        // QQueue<FileMetadata*> files;
        // QMutex files_m;
        // for (auto& f : vault->files){
        //     if (f->info.state == FileMetadata::PlainData)
        //         files.push_back(f);
        // }

        // // --- thread ---
        // int processorCount = Enviroment::GetInstance().ThreadCount();
        // int threadCount = files.size() < processorCount ? files.size() : processorCount;
        // QVector<QThread*> threads(threadCount);

        // // --- encrypt ---
        // for (int i = 0; i < threadCount; i++){
        //     threads[i] = QThread::create([&](){
        //         while(true){
        //             // --- flag check ---
        //             if (flag_suspend)
        //                 return;
        //             // --- get file ---
        //             files_m.lock();
        //             if (files.empty()){
        //                 files_m.unlock();
        //                 return;
        //             }
        //             FileMetadata* file = files.front();
        //             files.pop_front();
        //             files_m.unlock();
        //             // --- encrypt ---
        //             try{
        //                 // check header
        //                 if (file->header.iteration != vault->iteration ||
        //                     file->header.hmac != vault->hmac ||
        //                     file->header.salt != vault->globalSalt){
        //                     throw Error(Error::HeaderNotMatch, "Header does not matching");
        //                 }
        //                 // encrypt
        //                 Aes256EncryptFile(file, vault->aesKey);
        //                 success++;
        //             }catch(Error& e){
        //                 messages_m.lock();
        //                 flushMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
        //                 flushMessages.append("<font color='red'>  \"" + e.what() + "\"</font>");
        //                 errorMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
        //                 errorMessages.append("<font color='red'>  \"" + e.what() + "\"</font>");
        //                 messages_m.unlock();
        //                 failed++;
        //             }catch(std::exception& e){
        //                 messages_m.lock();
        //                 flushMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
        //                 flushMessages.append("<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"</font>");
        //                 errorMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to encrypt</font>");
        //                 errorMessages.append("<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"</font>");
        //                 messages_m.unlock();
        //                 failed++;
        //             }
        //         }
        //     });
        //     threads[i]->start();
        // }

        // // output
        // while (true){
        //     Utils::Sleep(500, 16, !flag_suspend);
        //     files_m.lock();
        //     if (files.empty()){
        //         files_m.unlock();
        //         emit signal_terminal_message(flushMessages);
        //         emit signal_progress(success + failed);
        //         break;
        //     }
        //     if (flag_suspend){
        //         files_m.unlock();
        //         break;
        //     }
        //     files_m.unlock();

        //     messages_m.lock();
        //     if (flushMessages.size()){
        //         qDebug() << "emitting message";
        //         emit signal_terminal_message(flushMessages);
        //         flushMessages.clear();
        //     }
        //     messages_m.unlock();
        //     emit signal_progress(success + failed);
        //     qDebug() << "emitting progress : " << success + failed;
        // }

        // // wait
        // for (int i = 0; i < threadCount; i++){
        //     if (threads[i]->isRunning()){
        //         threads[i]->wait();
        //     }
        // }

        // flushMessages.clear();
        // emit signal_terminal_clear();
        // if (success){
        //     flushMessages.append("<font  color='green'>" + QString::number(success) + "Files encrypted</font>");
        // }
        // if (failed){
        //     flushMessages.append("<font  color='green'>" + QString::number(success) + "Files failed to encrypt</font>");
        // }
        // emit signal_terminal_message(flushMessages);
        // if (failed){
        //     emit signal_terminal_message(errorMessages);
        // }
        // emit signal_done();
    }
    void AES256_Decrypt_All(Vault* vault){
        qDebug() << "empty function";
        // emit signal_start();
        // flag_suspend = false;
        // int processorCount = std::thread::hardware_concurrency() / 2;
        // processorCount += (processorCount == 0 ? 1 : 0);
        // int threadCount = vault->cipherIndex.size() < processorCount ? vault->cipherIndex.size() : processorCount;
        // QVector<QThread*> threads(threadCount);

        // std::atomic<int> success = 0;
        // std::atomic<int> failed = 0;
        // QStringList flushMessages;
        // QStringList errorMessages;
        // QMutex messages_m;
        // QQueue<file_t*> files;
        // QMutex files_m;
        // for (int i = 0; i < vault->files.size(); i++){
        //     if (vault->files[i].state == file_t::CipherData){
        //         files.push_back(&vault->files[i]);
        //     }
        // }

        // // work
        // for (int i = 0; i < threadCount; i++){
        //     threads[i] = QThread::create([&](){
        //         while(true){
        //             if (flag_suspend){
        //                 return;
        //             }
        //             files_m.lock();
        //             if (files.empty()){
        //                 files_m.unlock();
        //                 return;
        //             }
        //             file_t* file = files.front();
        //             files.pop_front();
        //             files_m.unlock();

        //             try{
        //                 AES256_Decrypt_File(file, vault->key);
        //                 success++;
        //             }catch(Error& e){
        //                 messages_m.lock();
        //                 flushMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to decrypt</font>");
        //                 flushMessages.append("<font color='red'>  \"" + e.what() + "\"</font>");
        //                 errorMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to decrypt</font>");
        //                 errorMessages.append("<font color='red'>  \"" + e.what() + "\"</font>");
        //                 messages_m.unlock();
        //                 failed++;
        //             }catch(std::exception& e){
        //                 messages_m.lock();
        //                 flushMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to decrypt</font>");
        //                 flushMessages.append("<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"</font>");
        //                 errorMessages.append("<font color='red'>\"" + file->absolutepath + "\" failed to decrypt</font>");
        //                 errorMessages.append("<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"</font>");
        //                 messages_m.unlock();
        //                 failed++;
        //             }
        //         }
        //     });
        //     threads[i]->start();
        // }

        // // output
        // while (true){
        //     Utils::Sleep(500, 16, !flag_suspend);
        //     files_m.lock();
        //     if (files.empty()){
        //         files_m.unlock();
        //         emit signal_terminal_message(flushMessages);
        //         emit signal_progress(success + failed);
        //         break;
        //     }
        //     if (flag_suspend){
        //         files_m.unlock();
        //         break;
        //     }
        //     files_m.unlock();

        //     messages_m.lock();
        //     if (flushMessages.size()){
        //         qDebug() << "emitting message";
        //         emit signal_terminal_message(flushMessages);
        //         flushMessages.clear();
        //     }
        //     messages_m.unlock();
        //     emit signal_progress(success + failed);
        //     qDebug() << "emitting progress : " << success + failed;
        // }

        // // wait
        // for (int i = 0; i < threadCount; i++){
        //     if (threads[i]->isRunning()){
        //         threads[i]->wait();
        //     }
        // }

        // flushMessages.clear();
        // emit signal_terminal_clear();
        // if (success){
        //     flushMessages.append("<font  color='green'>" + QString::number(success) + "Files encrypted</font>");
        // }
        // if (failed){
        //     flushMessages.append("<font  color='green'>" + QString::number(success) + "Files failed to encrypt</font>");
        // }
        // emit signal_terminal_message(flushMessages);
        // if (failed){
        //     emit signal_terminal_message(errorMessages);
        // }
        // emit signal_done();
    }

    // static -----------------------------------------------------
    inline static bool ReadFile(const QString& path, QByteArray& out)
    {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)){
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

    static void __AES256_Encrypt__(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
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
    static void __AES256_Decrypt__(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out)
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

    static void Aes256EncryptFile(FileMetadata* file, const QByteArray& key){
        // read file
        QByteArray plainData;
        if (!ReadFile(file->path.absolutepath, plainData))
            throw Error(Error::FailedToRead, "Failed to read : " + file->path.absolutepath);

        // reset iv
        file->header.iv.resize(16, 0);
        RAND_bytes(reinterpret_cast<unsigned char*>(file->header.iv.data()), 16);

        // encrypt
        QByteArray cipherData;
        __AES256_Encrypt__(plainData, key, file->header.iv, cipherData);

        // insert header
        cipherData.insert(0, file->header.Get());

        // save
        if (!WriteFile(file->path.absolutepath + ".enc", cipherData))
            throw Error(Error::FailedToSave, "Failed to save to: " + file->path.absolutepath);

        // delete
        QFile::remove(file->path.absolutepath);

        // set data
        file->path.absolutepath+= ".enc";
        file->path.relativePath+= ".enc";
        file->path.state = FileMetadata::CipherData;
    }

    static void Aes256DecryptFile(FileMetadata* file, const QByteArray& key){
        // read file
        QByteArray cipherData;
        if (!ReadFile(file->path.absolutepath, cipherData))
            throw Error(Error::FailedToRead, "Failed to read : " + file->path.absolutepath);

        // read header
        if (!file->header.Set(cipherData.mid(0, FileMetadata::Header::sizes::total))){
            throw Error(Error::HeaderNotMatch, "Header does not match");
        }
        cipherData.erase(cipherData.begin(), cipherData.begin() + FileMetadata::Header::sizes::total);

        // decrypt
        QByteArray plainData;
        __AES256_Decrypt__(cipherData, key, file->header.iv, plainData);

        // save
        if (!WriteFile(file->path.absolutepath.left(file->path.absolutepath.size() - 4), plainData))
            throw Error(Error::FailedToSave, "Failed to save to: " + file->path.absolutepath);

        // delete
        QFile::remove(file->path.absolutepath);

        // set data
        file->path.absolutepath = file->path.absolutepath.left(file->path.absolutepath.size() - 4);
        file->path.relativePath = file->path.relativePath.left(file->path.relativePath.size() - 4);
        file->path.state = FileMetadata::PlainData;
    }

    //--------------------------------------------------------------
    QThread* thread = nullptr;
    bool flag_suspend = false;

    void ProcessAll(Vault* vault, CRYPTIONMODE mode){
        emit signal_start();
        flag_suspend = false;

        // --- progress ---
        int targetSize;
        std::atomic<int> success = 0;
        std::atomic<int> failed = 0;

        // --- message ---
        QStringList flushMessages;
        QStringList errorMessages;
        QMutex messages_m;

        // --- files ---
        QQueue<FileMetadata*> files;
        QMutex files_m;
        for (auto& f : vault->files){
            if (mode == ENCRYPTION && f->path.state == FileMetadata::PlainData)
                files.push_back(f);
            else if (mode == DECRYPTION && f->path.state == FileMetadata::CipherData)
                files.push_back(f);
        }
        targetSize = files.size();

        // --- thread ---
        int processorCount = Enviroment::GetInstance().ThreadCount();
        int threadCount = files.size() < processorCount ? files.size() : processorCount;
        QVector<QThread*> threads(threadCount);

        // --- Process ---
        auto AppendToMessageQueue = [&messages_m, &flushMessages, &errorMessages](const QString& message1, const QString& message2){
            messages_m.lock();
            flushMessages.append(message1);
            flushMessages.append(message2);
            errorMessages.append(message1);
            errorMessages.append(message2);
            messages_m.unlock();
        };
        auto Process = [&](){
            while (true){
                // --- flag check ---
                if (flag_suspend)
                    return;
                // --- get file ---
                files_m.lock();
                if (files.empty()){
                    files_m.unlock();
                    return;
                }
                FileMetadata* file = files.front();
                files.pop_front();
                files_m.unlock();
                // --- encrypt ---
                try{
                    // check header
                    if (file->header.iteration != vault->iteration ||
                        file->header.hmac != vault->hmac ||
                        file->header.salt != vault->globalSalt
                        ){
                        throw Error(Error::HeaderNotMatch, "Header does not match");
                    }
                    // encrypt
                    if (mode == ENCRYPTION){
                        qDebug() << "encryptfile : " << file->path.relativePath;
                        //Aes256EncryptFile(file, vault->aesKey);
                    }else{
                        qDebug() << "decryptfile : " << file->path.relativePath;
                        //Aes256DecryptFile(file, vault->aesKey);
                    }
                    success++;
                }catch(Error& e){
                    QString message1 = "<font color='red'>\"" + file->path.absolutepath + "\" failed to " + (mode == ENCRYPTION ? "encrypt" :"decrypt") + "</font>";
                    QString message2 = "<font color='red'>  \"" + e.what() + "\"</font>";
                    AppendToMessageQueue(message1, message2);
                    failed++;
                }catch(std::exception& e){
                    QString message1 = "<font color='red'>\"" + file->path.absolutepath + "\" failed to " + (mode == ENCRYPTION ? "encrypt" :"decrypt") + "</font>";
                    QString message2 = "<font color='red'>  \"" + QString::fromStdString(e.what()) + "\"</font>";
                    AppendToMessageQueue(message1, message2);
                    failed++;
                }
            }
        };
        for (auto& thread : threads){
            thread = QThread::create([&](){
                Process();
            });
            thread->start();
        }

        // --- Wait ---
        while (targetSize - success - failed != 0){
            Utils::Sleep(500, 16, !flag_suspend);
            // Check queue
            files_m.lock();
            if (files.empty()){
                files_m.unlock();
                break;
            }
            files_m.unlock();
            // Emit queued message
            messages_m.lock();
            if (flushMessages.size()){
                emit signal_terminal_message(flushMessages);
                flushMessages.clear();
            }
            messages_m.unlock();
            // Emit progress
            emit signal_progress(success + failed);
        }

        // --- Emit done signal ---
        emit signal_terminal_clear();
        flushMessages.clear();
        if (success)    flushMessages.append("<font  color='green'>" + QString::number(success) + "Files " + (mode == ENCRYPTION ? "encrypted" : "decrypted") + "</font>");
        if (failed)     flushMessages.append("<font  color='green'>" + QString::number(success) + "Files failed to " + (mode == ENCRYPTION ? "encrypt" : "decrypt") + "</font>");
        emit signal_terminal_message(flushMessages);
        if (failed)     emit signal_terminal_message(errorMessages);
        emit signal_done();
    }

public slots:
    void StartEncryption(Vault* vault){
        // check
        if (thread && thread->isRunning()){
            QStringList msgs;
            msgs.append("Failed to start encryption");
            msgs.append("thread is running");
            emit signal_terminal_message(msgs);
            return;
        }
        delete thread;
        thread = nullptr;
        // start;
        thread = QThread::create([this](Vault* vault){
            ProcessAll(vault, ENCRYPTION);
        });
        thread->start();
    }
    void StartDecryption(Vault* vault){
        if (thread && thread->isRunning()){
            QStringList msgs;
            msgs.append("Failed to start decryption");
            msgs.append("thread is running");
            emit signal_terminal_message(msgs);
            return;
        }
        delete thread;
        thread = nullptr;
        // start;
        thread = QThread::create([this](Vault* vault){
            ProcessAll(vault, DECRYPTION);
        });
        thread->start();
    }
    void SuspendProcess(){
        flag_suspend = true;
    }

signals:
    void signal_terminal_clear();
    void signal_terminal_message(QStringList messages);
    void signal_progress(int value);
    void signal_start();
    void signal_done();
};




#endif // CRYPTO_HPP
