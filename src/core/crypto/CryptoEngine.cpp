#include "src/core/crypto/CryptoEngine.hpp"

#include <QVariant>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QStringList>
#include <atomic>
#include <QThread>

#include "src/core/FileInfo.hpp"
#include "src/core/Settings.hpp"

#define FONT_RED_PREFIX "<font color='red'>"
#define FONT_GREEN_PREFIX "<font color='green'>"
#define FONT_WHITE_PREFIX "<font color='white'>"
#define FONT_END_SUBFIX "</font>"

void Crypto::EncryptVault(Vault* vault){
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
    std::atomic<int>        queueSize;
    std::atomic<int>        failed = 0;
    std::atomic<int>        success = 0;


    // pre process file
    for (auto& file : vault->files){
        if (file->state == FileInfo::PlainData){
            file->SetHeader(vault->header.version, vault->header.salt, vault->header.iteration, vault->header.hmac);
            file->ResetIv();
            fileQueue.enqueue(file);
        }
    }
    queueSize = fileQueue.size();


    // Encrypt
    emit onEvent(Crypto::PROGRESS_MAX, QVariant(queueSize));
    emit onEvent(Crypto::PROGRESS_CURRENT, QVariant(0));
    emit onEvent(Crypto::MESSAGE_SINGLE, QVariant("Encrypting " FONT_GREEN_PREFIX + QString::number(queueSize) + FONT_END_SUBFIX " files"));
    emit onEvent(Crypto::START, QVariant());

    for (int i = 0; i < threadCount; i++){
        QThread* thread = QThread::create([&](){



        });
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start(QThread::TimeCriticalPriority);
    }



    // Wait for process and update progress



    // return;
    return;

abort:

final:
    vault->mutex.unlock();
    return;
}


void Crypto::DecryptVault(Vault* vault){



}





