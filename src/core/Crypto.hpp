#pragma once

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

#include "Vault.hpp"
#include "FileInfo.hpp"
#include "Utils.hpp"
#include "Settings.hpp"
#include "Error.hpp"


class Crypto : public QObject{
    Q_OBJECT
public:
    enum CryptionMode{
        MODE_ENCRYPTION,
        MODE_DECRYPTION
    };

    enum CryptoEvent{
        EVENT_START,
        EVENT_END,
        EVENT_MESSAGE,
        EVENT_PROGRESS,
        EVENT_FAILURE,
    };

    Crypto();
    ~Crypto();

private:
    static bool ReadFile(const QString& path, QByteArray& out);
    static bool WriteFile(const QString& path, const QByteArray& bytes);

    static bool __AES256_Encrypt__(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out, Error* error);
    static bool __AES256_Decrypt__(const QByteArray& input, const QByteArray& key, const QByteArray& iv, QByteArray& out, Error* error);

    static bool Aes256EncryptFile(FileInfo* file, const QByteArray& key, Error* error);
    static bool Aes256DecryptFile(FileInfo* file, const QByteArray& key, const QByteArray& hmac, Error* error);

public slots:
    void StartEncryption(Vault* vault);
    void StartDecryption(Vault* vault);
    void SuspendProcess();

signals:
    void onEvent(CryptoEvent event, CryptionMode mode, int intVal);
    void onMessageQueueReady(CryptoEvent event, CryptionMode mode, QStringList messages);

private:
    void AES256_Encrypt_All(Vault* vault);
    void AES256_Decrypt_All(Vault* vault);

    QThread* thread = nullptr;
    bool flag_suspend = false;
};
