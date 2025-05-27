#include "src/core/cryptography/CryptoEngine.hpp"

#include <QVariant>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QStringList>
#include <atomic>
#include <QThread>

#include "src/core/fileinfo/FileInfo.hpp"
#include "src/core/Settings.hpp"
#include "Cryptography.hpp"

#define FONT_RED_PREFIX "<font color='red'>"
#define FONT_GREEN_PREFIX "<font color='green'>"
#define FONT_WHITE_PREFIX "<font color='white'>"
#define FONT_END_SUBFIX "</font>"

CryptoEngine::CryptoEngine(QWidget *parent) : QObject(parent){

};

void CryptoEngine::AES256EncryptFile(FileInfo *file, QByteArray &key, Error& error){
    // read
    QFile f(file->path.absolutepath);
    if (!f.exists()){
        error.Set(Error::IO_NOT_EXISTS, "File not exists", file->path.absolutepath);
        return;
    }
    if (!f.open(QFile::ReadOnly)){
        error.Set(Error::IO_READ_FAILURE, "Failed to read", file->path.absolutepath);
        return;
    }
    QByteArray plainData = f.readAll();
    QByteArray cipherData;
    f.close();

    // encrypt
    Cryptography::AES_256_Encrypt(plainData, key, file->header.iv, cipherData, error);
    if (error.code() != Error::NO_ERROR)
        return;

    // insert header
    cipherData.insert(0, file->header.GetData());

    // save
    f.setFileName(file->path.absolutepath + ".enc");
    if (!f.open(QFile::WriteOnly)){
        error.Set(Error::IO_WRITE_FAILURE, "Failed to save", file->path.absolutepath);
        return;
    }
    f.write(cipherData);
    f.close();

    // delete plain file
    QFile::remove(file->path.absolutepath);

    // set data
    file->path.absolutepath += ".enc";
    file->path.relativePath += ".enc";
    file->state = FileInfo::CipherData;
    return;
}

void CryptoEngine::AES256DecryptFile(FileInfo *file, QByteArray &key, const QByteArray &hmac, Error& error){
    // read
    QFile f(file->path.absolutepath);
    if (!f.exists()){
        error.Set(Error::IO_NOT_EXISTS, "File not exists", file->path.absolutepath);
        return;
    }
    if (!f.open(QFile::ReadOnly)){
        error.Set(Error::IO_READ_FAILURE, "Failed to read", file->path.absolutepath);
        return;
    }
    QByteArray cipherData = f.readAll();
    QByteArray plainData;
    f.close();

    // load and check header
    if (!file->SetHeader(cipherData)){
        file->integrity = false;
        error.Set(Error::HEADER_SIGNATURE_FAILURE, "File has been corrupted", file->path.absolutepath);
        return;
    }
    if (file->header.hmac != hmac){
        file->isHeaderMatch = false;
        error.Set(Error::HEADER_HMAC_FAILURE, "Header hmac does not match", file->path.absolutepath);
        return;
    }

    // remove header
    cipherData.remove(0, FileInfo::Sizes::total);

    // decrypt
    Cryptography::AES_256_Decrypt(plainData, key, file->header.iv, cipherData, error);
    if (error.code() != Error::NO_ERROR)
        return;

    // save
    f.setFileName(file->path.absolutepath.left(file->path.absolutepath.size() - 4));
    if (!f.open(QFile::WriteOnly)){
        error.Set(Error::IO_WRITE_FAILURE, "Failed to save", file->path.absolutepath);
        return;
    }
    f.write(plainData);
    f.close();

    // delete
    QFile::remove(file->path.absolutepath);

    // set data
    file->path.absolutepath = file->path.absolutepath.left(file->path.absolutepath.size() - 4);
    file->path.relativePath = file->path.relativePath.left(file->path.relativePath.size() - 4);
    file->state = FileInfo::PlainData;
    return;
}

void CryptoEngine::EncryptVault(Vault* vault){
    run = true;
    // check vault and lock
    if (!vault || !vault->mutex.tryLock()){
        emit onEvent(MESSAGE_SINGLE, QVariant("cannot access vault"));
        emit onEvent(ABORT, QVariant());
        return;
    }

    //
    QQueue<FileInfo*>       fileQueue;
    QMutex                  queueMutex;
    QStringList             flushBuffer;
    QStringList             finalMessage;
    QMutex                  messageMutex;
    int                     threadCount = Settings::GetInstance().GetThreadCount();
    std::atomic<int>        failed = 0;
    std::atomic<int>        success = 0;
    std::atomic<int>        joinedThread = 0;

    QByteArray              version = vault->aes.FormatVersion();
    QByteArray              salt = vault->aes.GlobalSalt();
    int                     iteration = vault->aes.Iteration();
    QByteArray              hmac = vault->aes.Hmac();
    QByteArray              aesKey = vault->aes.AesKey();

    // pre process file
    for (auto& file : vault->files){
        if (file->state == FileInfo::PlainData){
            file->SetHeader(version, salt, iteration, hmac);
            file->ResetIv();
            fileQueue.enqueue(file);
        }
    }

    // Encrypt
    emit onEvent(PROGRESS_MAX, QVariant(fileQueue.size()));
    emit onEvent(PROGRESS_CURRENT, QVariant(0));
    emit onEvent(MESSAGE_SINGLE, QVariant("Encrypting " FONT_GREEN_PREFIX + QString::number(fileQueue.size()) + FONT_END_SUBFIX " files"));
    emit onEvent(START, QVariant());

    for (int i = 0; i < threadCount; i++){
        QThread* thread = QThread::create([&](){
            while(true){
                FileInfo* file;
                {
                    QMutexLocker<QMutex> lock(&queueMutex);
                    if (fileQueue.isEmpty() || !run) goto final;
                    file = fileQueue.dequeue();
                }

                // Encrypt
                Error e;
                AES256EncryptFile(file, aesKey, e);
                if (e.code() == Error::NO_ERROR){
                    ++success;
                }else{
                    QMutexLocker<QMutex> messageLock(&messageMutex);
                    finalMessage.append(e.what() + " : " + e.path());
                    flushBuffer.append(e.what() + " : " + e.path());
                    ++failed;
                }
            }
        final:
            joinedThread++;
            return;
        });
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }

    // Wait for process and update progress
    while (threadCount != joinedThread){
        // flush message
        {
            QMutexLocker<QMutex> lock(&messageMutex);
            if (flushBuffer.size() != 0){
                emit onEvent(MESSAGE_LIST, QVariant(flushBuffer));
                flushBuffer.clear();
            }
            emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
        }

        // sleep for 10 times (1 sec)
        for (int i = 0; i < 10; i++){
            if (threadCount == joinedThread) break;
            QThread::msleep(100);
        }
    }

    // final
    emit onEvent(MESSAGE_LIST, QVariant(finalMessage));
    emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
    emit onEvent(END, QVariant());
    return;
}

void CryptoEngine::DecryptVault(Vault* vault){
    run = true;
    // check vault and lock
    if (!vault || !vault->mutex.tryLock()){
        emit onEvent(MESSAGE_SINGLE, QVariant("cannot access vault"));
        emit onEvent(ABORT, QVariant());
        return;
    }

    //
    QQueue<FileInfo*>       fileQueue;
    QMutex                  queueMutex;
    QStringList             flushBuffer;
    QStringList             finalMessage;
    QMutex                  messageMutex;
    int                     threadCount = Settings::GetInstance().GetThreadCount();
    std::atomic<int>        failed = 0;
    std::atomic<int>        success = 0;
    std::atomic<int>        joinedThread = 0;

    QByteArray              hmac = vault->aes.Hmac();
    QByteArray              aesKey = vault->aes.AesKey();

    // pre process file
    for (auto& file : vault->files){
        if (file->state == FileInfo::CipherData){
            fileQueue.enqueue(file);
        }
    }

    // Decrypt
    emit onEvent(PROGRESS_MAX, QVariant(fileQueue.size()));
    emit onEvent(PROGRESS_CURRENT, QVariant(0));
    emit onEvent(MESSAGE_SINGLE, QVariant("Decrypting " FONT_GREEN_PREFIX + QString::number(fileQueue.size()) + FONT_END_SUBFIX " files"));
    emit onEvent(START, QVariant());

    for (int i = 0; i < threadCount; i++){
        QThread* thread = QThread::create([&](){
            while(true){
                FileInfo* file;
                {
                    QMutexLocker<QMutex> lock(&queueMutex);
                    if (fileQueue.isEmpty() || !run) goto final;
                    file = fileQueue.dequeue();
                }

                // Decrypt
                Error e;
                AES256DecryptFile(file, aesKey, hmac, e);
                if (e.code() == Error::NO_ERROR){
                    ++success;
                }else{
                    QMutexLocker<QMutex> messageLock(&messageMutex);
                    finalMessage.append(e.what() + " : " + e.path());
                    flushBuffer.append(e.what() + " : " + e.path());
                    ++failed;
                }
            }
        final:
            ++joinedThread;
            return;
        });
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }

    // Wait for process and update progress
    while (threadCount != joinedThread){
        // flush message
        {
            QMutexLocker<QMutex> lock(&messageMutex);
            if (flushBuffer.size() != 0){
                emit onEvent(MESSAGE_LIST, QVariant(flushBuffer));
                flushBuffer.clear();
            }
            emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
        }

        // sleep for 10 times (1 sec)
        for (int i = 0; i < 10; i++){
            if (threadCount == joinedThread) break;
            QThread::msleep(100);
        }
    }

    // final
    emit onEvent(MESSAGE_LIST, QVariant(finalMessage));
    emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
    emit onEvent(END, QVariant());
    return;
}

void CryptoEngine::SuspendProcess()
{
    run = false;
}
