#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>

#include "src/core/vault/Vault.hpp"
#include "Error.hpp"

class CryptoEngine : public QObject{
    Q_OBJECT
public:
    explicit CryptoEngine(QWidget *parent = nullptr);
    enum Event{
        START,
        END,
        ABORT,
        PROGRESS_CURRENT,
        PROGRESS_MAX,
        MESSAGE_SINGLE,
        MESSAGE_LIST,
    };

private:
    void AES256EncryptFile(FileInfo* file, QByteArray& key, Error& error);
    void AES256DecryptFile(FileInfo* file, QByteArray& key, const QByteArray& hmac, Error& error);

signals:
    void onEvent(Event event, QVariant param);

public slots:
    void AES256EncryptFiles(QQueue<FileInfo*> files);
    void AES256DecryptFiles(QQueue<FileInfo*> files);
    void SuspendProcess();

    void EncryptVault(Vault* vault);
    void DecryptVault(Vault* vault);
private:
    bool run;
};






