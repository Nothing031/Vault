#pragma once

#include <QVector>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QException>
#include <QElapsedTimer>
#include <QApplication>
#include <QDirListing>
#include <QMutex>
#include <QDirIterator>
#include <QCryptographicHash>

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "FileInfo.hpp"
#include "Settings.hpp"

namespace fs = std::filesystem;

class Vault{
    inline static const QString extension = ".enc";

public:
    QDir directory;
    QVector<FileInfo*> files;
    QMutex          mutex;

    QByteArray      aesKey;
    bool            unlocked = false;

    QString version;
    QByteArray hmac;
    QByteArray globalSalt;
    int iteration;

    Vault();
    Vault(const Vault& other);
    ~Vault();
    Vault& operator=(const Vault& other);


    static Vault* Create(const QString& dir, const QString& password, const int& iteration = ITERATION);
    static Vault* Load(const QString& dir);
    bool SaveDataFile();

public slots:
    void VerifyPassword(const QString& password);

    void LoadFiles();

private:
    static bool _CreateKey(const QString& str, const QByteArray& salt, const int& itr, QByteArray& out);

};


