#ifndef VAULT_H
#define VAULT_H

#include "openssl/evp.h"
#include <filesystem>

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

typedef struct __FILE_INFO__{
    QFileInfo file;
    QString relativePath;
    bool encrypted;
}FILE_INFO;

class Backup{
private:
    QDir dir;
public:

};

class Vault{
public:
    Backup *backup;
    QDir dir;
    QString display_name;
    QString password;
    QByteArray key;

    QMutex mutex;
    QVector<FILE_INFO> files;
    QVector<int> index_encrypted;
    QVector<int> index_decrypted;

    Vault(){

    }
    Vault(const Vault& other)
    {
        backup = other.backup;
        dir = other.dir;
        display_name = other.display_name;
        password = other.password;
        files = other.files;
        index_encrypted = other.index_encrypted;
        index_decrypted = other.index_decrypted;
    }
    Vault& operator=(const Vault& other)
    {
        if (this == &other)
            return *this;
        backup = other.backup;
        dir = other.dir;
        display_name = other.display_name;
        password = other.password;
        files = other.files;
        index_encrypted = other.index_encrypted;
        index_decrypted = other.index_decrypted;
        return *this;
    }

    void CreateKey(const QString& input)
    {
        qDebug() << "[VAULT]  Generating key";
        QApplication::processEvents();
        QElapsedTimer timer;
        timer.start();

        key.resize(32);

        if(PKCS5_PBKDF2_HMAC_SHA1(
            (const char*)input.constData(),
            input.size(),
            nullptr,
            0,
            600000,
            32,
            (unsigned char*)key.data()
        )){
            qDebug() << "[VAULT]  Key Created";
            qDebug() << "  Elapsed time :" << timer.elapsed() << "ms";
            qDebug() << "  Key          :" << key.toHex();
        }else{
            qDebug() << "[VAULT]  ERROR Key generation failure";
        }
    }
    void UpdateIndex()
    {
        QElapsedTimer timer;
        timer.start();
        qDebug() << "[VAULT] indexing...";
        QApplication::processEvents();
        index_encrypted.clear();
        index_decrypted.clear();
        for (int i = 0; i < files.size(); i++){
            if (files[i].encrypted)
                index_encrypted.push_back(i);
            else
                index_decrypted.push_back(i);
        }
        qDebug() << "[VAULT] Indexing done" << timer.elapsed() << "ms";
        qDebug() << "  Elapsed time    :" << timer.elapsed() << "ms";
        qDebug() << "  Encrypted files :" << index_encrypted.size();
        qDebug() << "  Decrypted files :" << index_decrypted.size();
        qDebug() << "  Total           :" << files.size();
    }
    bool LoadFiles()
    {
        QElapsedTimer timer;
        timer.start();
        qDebug() << "[VAULT] loading files...";
        QApplication::processEvents();
        if (dir.exists()){
            files.clear();
            for (const auto& file : std::filesystem::recursive_directory_iterator(dir.path().toStdWString())){
                QFileInfo qinfo = QFileInfo(file.path());
                FILE_INFO file_info;
                file_info.file = qinfo;
                file_info.relativePath = dir.relativeFilePath(qinfo.absoluteFilePath());
                file_info.encrypted = (qinfo.path().endsWith(".enc") ? true : false);
                files.push_back(file_info);
            }

            std::sort(files.begin(), files.end(), [](const FILE_INFO& a, const FILE_INFO& b){
                return a.relativePath < b.relativePath;
            });

            qDebug() << "[VAULT] Loading done";
            qDebug() << "  Elapsed time :" << timer.elapsed() << "ms";
            qDebug() << "  Files        :" << files.size();
            UpdateIndex();
            return true;
        }else{
            qDebug() << "[VAULT] ERROR directory not exists";
            return false;
        }
    }
};


#endif // VAULT_H
