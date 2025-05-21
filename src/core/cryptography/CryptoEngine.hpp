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
    void EncryptFile(FileInfo* file, QByteArray& key, const FileInfo::FileHeader& header, Error& error);
    void DecryptFile(FileInfo* file, QByteArray& key, const QByteArray& hmac, Error& error);

signals:
    void onEvent(Event event, QVariant param);

public slots:
    void EncryptVault(Vault* vault);
    void DecryptVault(Vault* vault);
    void SuspendProcess();

private:
    bool run;
};






