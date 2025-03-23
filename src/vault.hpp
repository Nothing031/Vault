#ifndef VAULT_H
#define VAULT_H

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

#include <filesystem>

#include <openssl/evp.h>

#include "file_t.hpp"

class Vault{
public:
    QDir backup_dir;
    QDir dir;
    QString display_name;
    QString password;
    QByteArray key;

    QMutex mutex;
    QVector<file_t> files;
    QVector<int> index_cipher;
    QVector<int> index_plain;

    Vault(){

    }
    Vault(const Vault& other)
    {
        backup_dir = other.backup_dir;
        dir = other.dir;
        display_name = other.display_name;
        password = other.password;
        files = other.files;
        index_cipher = other.index_cipher;
        index_plain = other.index_plain;
    }
    Vault& operator=(const Vault& other)
    {
        if (this == &other)
            return *this;
        backup_dir = other.backup_dir;
        dir = other.dir;
        display_name = other.display_name;
        password = other.password;
        files = other.files;
        index_cipher = other.index_cipher;
        index_plain = other.index_plain;
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
            100000,
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
        index_cipher.clear();
        index_plain.clear();
        for (int i = 0; i < files.size(); i++){
            if (files[i].state == file_t::CipherData)
                index_cipher.push_back(i);
            else
                index_plain.push_back(i);
        }
        qDebug() << "[VAULT] Indexing done" << timer.elapsed() << "ms";
        qDebug() << "  Elapsed time    :" << timer.elapsed() << "ms";
        qDebug() << "  Encrypted files :" << index_cipher.size();
        qDebug() << "  Decrypted files :" << index_plain.size();
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
                file_t filet;
                filet.absolutepath = qinfo.absoluteFilePath();
                filet.relativePath = dir.relativeFilePath(qinfo.absoluteFilePath());
                filet.state = (qinfo.path().endsWith(".enc") ? file_t::CipherData : file_t::PlainData);
                files.push_back(filet);
            }

            std::sort(files.begin(), files.end(), [](const file_t& a, const file_t& b){
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
