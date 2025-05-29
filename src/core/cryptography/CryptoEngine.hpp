#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>

#include "src/core/vault/Vault.hpp"
#include "Error.hpp"
#include "src/core/vault/AES256Settings.hpp"

class CryptoEngine : public QObject{
    Q_OBJECT
public:
    explicit CryptoEngine(QWidget *parent = nullptr);
    ~CryptoEngine();
    enum Event{
        START,
        END,
        PROGRESS_CURRENT,
        PROGRESS_MAX,
        MESSAGE_SINGLE,
        MESSAGE_LIST,
        CLEAR_TERMINAL
    };


private:
    void AES256EncryptFile(FileInfo* file, const AES256Settings &aes, Error& error);
    void AES256DecryptFile(FileInfo* file, const AES256Settings &aes, Error& error);

public:
    void AES256EncryptFiles(QQueue<FileInfo*> &files, QMutex* mutex, const AES256Settings aes);
    void AES256DecryptFiles(QQueue<FileInfo*> &files, QMutex* mutex, const AES256Settings aes);
signals:
    void onEvent(Event event, QVariant param);

public slots:
    void SuspendProcess();

private:
    bool run = false;
};






