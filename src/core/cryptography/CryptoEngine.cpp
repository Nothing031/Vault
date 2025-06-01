#include "src/core/cryptography/CryptoEngine.hpp"

#include <QVariant>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QStringList>
#include <atomic>
#include <thread>
#include <QThread>
#include <QObject>
#include <QWidget>

#include "src/core/fileinfo/FileInfo.hpp"
#include "src/core/fileinfo/FileHeader.hpp"
#include "Cryptography.hpp"

#define RED_PREFIX "<font color='red'>"
#define GREEN_PREFIX "<font color='green'>"
#define WHITE_PREFIX "<font color='white'>"
#define END_SUBFIX "</font>"

CryptoEngine::CryptoEngine(std::shared_ptr<QQueue<std::shared_ptr<FileInfo>>> pFiles, QMutex* mutex,const AES256Settings& aes, QWidget *parent)
    : QObject(parent)
{
    this->pFiles = pFiles;
    this->pMutex = mutex;
    this->aes = aes;
}

CryptoEngine::~CryptoEngine()
{

}

void CryptoEngine::AES256EncryptFile(std::shared_ptr<FileInfo> file, const AES256Settings &aes, Error& error){
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

    // set header
    FileHeader header;
    header.SetData(aes);
    header.RandIv();

    // encrypt
    Cryptography::AES_256_Encrypt(plainData, aes.AesKey(), header.iv, cipherData, error);
    if (error.code() != Error::CLEAN)
        return;

    // insert header
    cipherData.insert(0, header.Serialize());

    // save
    f.setFileName(file->path.absolutepath + ".enc");
    if (!f.open(QFile::WriteOnly)){
        error.Set(Error::IO_WRITE_FAILURE, "Failed to save", file->path.absolutepath);
        return;
    }
    f.write(cipherData);
    f.close();

    // delete plain file
    if (!QFile::remove(file->path.absolutepath)){
        error.Set(Error::IO_DELETE_FAILURE, "Failed to delete origin file, file is open in unknown process", file->path.absolutepath);
        QFile::remove(file->path.absolutepath + ".enc");
        return;
    }

    // set data
    file->path.absolutepath += ".enc";
    file->state = FileInfo::CIPHER_GOOD;
    return;
}

void CryptoEngine::AES256DecryptFile(std::shared_ptr<FileInfo> file, const AES256Settings &aes, Error& error){
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

    // load header
    FileHeader header;
    FileInfo::State state = header.Deserialize(cipherData, aes.Hmac());
    if (state != FileInfo::CIPHER_GOOD){
        file->state = state;
        error.Set(Error::HEADER_HMAC_FAILURE, "File hmac does not match or the file has been corrupted", file->path.absolutepath);
        return;
    }

    // remove header
    cipherData.remove(0, FileHeader::Sizes::total);

    // decrypt
    Cryptography::AES_256_Decrypt(cipherData, aes.AesKey(), header.iv, plainData, error);
    if (error.code() != Error::CLEAN)
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
    if (!QFile::remove(file->path.absolutepath)){
        error.Set(Error::IO_DELETE_FAILURE, "Failed to delete origin file, file is open in unknown process", file->path.absolutepath);
        QFile::remove(file->path.absolutepath.left(file->path.absolutepath.size() - 4));
        return;
    }

    // set data
    file->path.absolutepath = file->path.absolutepath.left(file->path.absolutepath.size() - 4);
    file->state = FileInfo::PLAIN_GOOD;
    return;
}

void CryptoEngine::AES256EncryptFiles()
{
    run = true;
    // check
    if (pFiles->empty()){
        emit onEvent(MESSAGE_SINGLE, QVariant("No any file to encrypt"));
        return;
    }
    if (!pMutex->tryLock(1000)){
        emit onEvent(MESSAGE_SINGLE, QVariant(RED_PREFIX "Error failed to start encryption" END_SUBFIX));
        emit onEvent(MESSAGE_SINGLE, QVariant("The vault is already on progress"));
        return;
    }
    emit onEvent(CLEAR_TERMINAL, QVariant());
    QMutex                  queueMutex;

    QStringList             flushBuffer;
    QStringList             finalMessage;
    QMutex                  messageMutex;

    int                     threadCount = std::thread::hardware_concurrency();
    std::atomic<int>        failed = 0;
    std::atomic<int>        success = 0;
    std::atomic<int>        joinedThread = 0;

    // emit messages
    emit onEvent(PROGRESS_MAX, QVariant(pFiles->size()));
    emit onEvent(PROGRESS_CURRENT, QVariant(0));
    QString _startMessage = pFiles->size() == 1 ? "Encrypting file" : "Encrypting " GREEN_PREFIX + QString::number(pFiles->size()) + END_SUBFIX " files";
    emit onEvent(MESSAGE_SINGLE, QVariant(_startMessage));
    emit onEvent(START, QVariant());

    for (int i = 0; i < threadCount; i++){
        QThread* thread = QThread::create([&](){
            while(true){
                std::shared_ptr<FileInfo> file;
                {   // queue mutex scope
                    QMutexLocker<QMutex> lock(&queueMutex);
                    if (pFiles->isEmpty() || !run) goto final;
                    file = pFiles->dequeue();
                }

                // Encrypt
                Error e;
                AES256EncryptFile(file, aes, e);
                if (e.code() == Error::CLEAN){
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

    // emit events
    bool flag = true;
    while(flag){
        {   // message mutex scope
            QMutexLocker<QMutex> lock(&messageMutex);
            if (flushBuffer.size() != 0){
                emit onEvent(MESSAGE_LIST, QVariant(flushBuffer));
                flushBuffer.clear();
            }
            emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
        }
        // 1 sec interval
        for (int i = 0; i < 10; i++){
            QThread::msleep(100);
            if (threadCount == joinedThread){
                flag = false;
                break;
            }
        }
    }

    // done
    if (success){
        finalMessage.append(GREEN_PREFIX + QString::number(success) + END_SUBFIX " files has been successfully encrypted");
    }
    if (failed){
        finalMessage.append("Failed to encrypt " RED_PREFIX + QString::number(failed) + END_SUBFIX " files");
    }

    emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
    emit onEvent(END, QVariant());
    emit onEvent(MESSAGE_LIST, QVariant(finalMessage));
    pMutex->unlock();
    emit finished();
}

void CryptoEngine::AES256DecryptFiles()
{
    run = true;
    // check
    if (pFiles->empty()){
        emit onEvent(MESSAGE_SINGLE, QVariant("No any file to decrypt"));
        return;
    }
    if (!pMutex->tryLock(1000)){
        emit onEvent(MESSAGE_SINGLE, QVariant(RED_PREFIX "Error failed to start decryption" END_SUBFIX));
        emit onEvent(MESSAGE_SINGLE, QVariant("The vault is already on progress"));
        return;
    }
    emit onEvent(CLEAR_TERMINAL, QVariant());
    QMutex                  queueMutex;

    QStringList             flushBuffer;
    QStringList             finalMessage;
    QMutex                  messageMutex;

    int                     threadCount = std::thread::hardware_concurrency();
    std::atomic<int>        failed = 0;
    std::atomic<int>        success = 0;
    std::atomic<int>        joinedThread = 0;

    // emit messages
    emit onEvent(PROGRESS_MAX, QVariant(pFiles->size()));
    emit onEvent(PROGRESS_CURRENT, QVariant(0));
    QString _startMessage = pFiles->size() == 1 ? "Decrypting file" : "Decrypting " GREEN_PREFIX + QString::number(pFiles->size()) + END_SUBFIX " files";
    emit onEvent(MESSAGE_SINGLE, QVariant(_startMessage));
    emit onEvent(START, QVariant());

    // encrypt
    for (int i = 0; i < threadCount; i++){
        QThread* thread = QThread::create([&](){
            while(true){
                std::shared_ptr<FileInfo> file;
                {   // queue mutex scope
                    QMutexLocker<QMutex> lock(&queueMutex);
                    if (pFiles->isEmpty() || !run) goto final;
                    file = pFiles->dequeue();
                }

                // Encrypt
                Error e;
                AES256DecryptFile(file, aes, e);
                if (e.code() == Error::CLEAN){
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

    // emit events
    bool flag = true;
    while(flag){
        {   // message mutex scope
            QMutexLocker<QMutex> lock(&messageMutex);
            if (flushBuffer.size() != 0){
                emit onEvent(MESSAGE_LIST, QVariant(flushBuffer));
                flushBuffer.clear();
            }
            emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
        }
        // 1 sec interval
        for (int i = 0; i < 10; i++){
            QThread::msleep(100);
            if (threadCount == joinedThread){
                flag = false;
                break;
            }
        }
    }

    // done
    if (success){
        finalMessage.append(GREEN_PREFIX + QString::number(success) + END_SUBFIX " files has been successfully decrypted");
    }
    if (failed){
        finalMessage.append("Failed to decrypt " RED_PREFIX + QString::number(failed) + END_SUBFIX " files");
    }

    emit onEvent(PROGRESS_CURRENT, QVariant(success + failed));
    emit onEvent(END, QVariant());
    emit onEvent(MESSAGE_LIST, QVariant(finalMessage));
    pMutex->unlock();
    emit finished();
}

void CryptoEngine::SuspendProcess()
{
    run = false;
}
