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
    explicit CryptoEngine(std::shared_ptr<QQueue<std::shared_ptr<FileInfo>>> pFiles, QMutex* mutex,const AES256Settings& aes, QWidget *parent = nullptr);
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
    void AES256EncryptFile(std::shared_ptr<FileInfo> file, const AES256Settings &aes, Error& error);
    void AES256DecryptFile(std::shared_ptr<FileInfo> file, const AES256Settings &aes, Error& error);

public slots:
    void AES256EncryptFiles();
    void AES256DecryptFiles();

signals:
    void onEvent(Event event, QVariant param);
    void finished();

public slots:
    void SuspendProcess();

private:
    bool run = false;
    QMutex* pMutex;
    std::shared_ptr<QQueue<std::shared_ptr<FileInfo>>> pFiles;
    AES256Settings aes;
};






