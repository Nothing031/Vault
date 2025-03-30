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
    QDir directory;
    QDir backupDir;
    QString encryptionExtension;
    QString displayName;
    QString password;
    QByteArray key;

    QVector<file_t> files;
    QVector<int> cipherIndex;
    QVector<int> plainIndex;

    Vault(){

    }
    Vault(const QString& directory, const QString& password, const int& backupLen){
        this->directory =  QDir(directory);
        this->backupDir = QDir(directory + "/" + password.left(backupLen));
        this->displayName =  directory.dirName();
        this->password = password;
        this->key = {};

        this->files = {};
        this->cipherIndex = {};
        this->plainIndex = {};
        this->id = -1;
    }

    Vault(const Vault& other)
    {
        backupDir = other.backupDir;
        directory = other.directory;
        displayName = other.displayName;
        password = other.password;
        files = other.files;
        cipherIndex = other.cipherIndex;
        plainIndex = other.plainIndex;
    }
    Vault& operator=(const Vault& other)
    {
        if (this == &other)
            return *this;
        backupDir = other.backupDir;
        directory = other.directory;
        displayName = other.displayName;
        password = other.password;
        files = other.files;
        cipherIndex = other.cipherIndex;
        plainIndex = other.plainIndex;
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
        cipherIndex.clear();
        plainIndex.clear();
        for (int i = 0; i < files.size(); i++){
            if (files[i].state == file_t::CipherData)
                cipherIndex.push_back(i);
            else
                plainIndex.push_back(i);
        }
        qDebug() << "[VAULT] Indexing done";
        qDebug() << "  Elapsed time    :" << timer.elapsed() << "ms";
        qDebug() << "  Encrypted files :" << cipherIndex.size();
        qDebug() << "  Decrypted files :" << plainIndex.size();
        qDebug() << "  Total           :" << files.size();
    }

    bool LoadFiles()
    {
        QElapsedTimer timer;
        timer.start();
        qDebug() << "[VAULT] loading files...";
        QApplication::processEvents();
        if (directory.exists()){
            files.clear();
            try{
                for (const auto& file : std::filesystem::recursive_directory_iterator(directory.path().toStdWString())){
                    if (std::string::npos != file.path().generic_wstring().find(this->backupDir.path().toStdWString())){
                        qDebug() << "backup file";
                        continue;
                    }
                    if (file.is_regular_file()){
                        QFileInfo qinfo = QFileInfo(file.path());
                        file_t filet;
                        filet.displayPath = qinfo.fileName();
                        filet.absolutepath = qinfo.absoluteFilePath();
                        filet.relativePath = directory.relativeFilePath(qinfo.absoluteFilePath());
                        filet.state = (qinfo.fileName().endsWith(".enc", Qt::CaseInsensitive) ? file_t::CipherData : file_t::PlainData);
                        if (filet.state == file_t::CipherData){
                            filet.displayPath = filet.displayPath.left(filet.displayPath.size() - 4);
                        }
                        files.push_back(filet);
                    }
                }
            } catch(std::exception& e){
                qDebug() << e.what();
                return false;
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
