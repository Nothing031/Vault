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
#include <QCryptographicHash>

#include <filesystem>

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "filemetadata.hpp"
#include "enviroment.hpp"

namespace fs = std::filesystem;


class Vault{
    inline static const QString extension = ".enc";

public:
    QDir directory;
    QVector<FileMetadata*> files;

    QMutex          mutex;

    bool            unlocked = false;
    QString         password;
    QString         version;
    QByteArray      globalSalt;
    int             iteration;
    QByteArray      hmac;
    QByteArray      aesKey;

    Vault()
    {
        directory   = {};
        files       = {};
        password    = {};
        version     = ENV_VERSION;
        globalSalt  = {};
        iteration   = ENV_DEFAULT_ITERATION;
        hmac        = {};
    }

    ~Vault(){
        for (int i = 0; i < files.size(); i++){
            delete files[i];
        }
    }

    Vault(const Vault& other)
    {
        directory   = other.directory;
        files       = other.files;
        unlocked    = other.unlocked;
        password    = other.password;
        version     = other.version;
        globalSalt  = other.globalSalt;
        iteration   = other.iteration;
        hmac        = other.hmac;
        aesKey      = other.aesKey;
    }

    Vault& operator=(const Vault& other)
    {
        if (this == &other)
            return *this;
        directory   = other.directory;
        files       = other.files;
        unlocked    = other.unlocked;
        password    = other.password;
        version     = other.version;
        globalSalt  = other.globalSalt;
        iteration   = other.iteration;
        hmac        = other.hmac;
        aesKey      = other.aesKey;
        return *this;
    }

    static Vault Load(QString vaultDataPath)
    {





    }

    static Vault Create(const QString& dir, const QString& password, const int& iteration = ENV_DEFAULT_ITERATION)
    {
        Vault vault;
        vault.directory = QDir(dir);
        vault.password = password;
        vault.iteration = iteration;

        vault.globalSalt.resize(FileMetadata::Header::sizes::salt);
        RAND_bytes(reinterpret_cast<unsigned char*>(vault.globalSalt.data()),
                   FileMetadata::Header::sizes::salt
                   );

        // create key;
        QByteArray pbkdf2Key(64, 0);
        if (PKCS5_PBKDF2_HMAC(reinterpret_cast<const char*>(vault.password.constData()),
                          vault.password.size(),
                          reinterpret_cast<const unsigned char*>(vault.globalSalt.constData()),
                          vault.globalSalt.size(),
                          vault.iteration,
                          EVP_sha256(),
                          64,
                          reinterpret_cast<unsigned char*>(pbkdf2Key.data())
                              )){
            vault.hmac = pbkdf2Key.mid(0, 32);
            // aesKey = pbkdf2Key.mid(32, 32);
            return vault;
        }else{
            throw std::exception("failed to create key");
        }
    }

    bool VerifyPassword(const QString& password){
        QElapsedTimer timer;
        timer.start();
        qDebug() << "Creating key...";

        QByteArray pbkdf2Key(64, 0);
        if (PKCS5_PBKDF2_HMAC(reinterpret_cast<const char*>(password.constData()),
                              password.size(),
                              reinterpret_cast<const unsigned char*>(this->globalSalt.constData()),
                              this->globalSalt.size(),
                              this->iteration,
                              EVP_sha256(),
                              64,
                              reinterpret_cast<unsigned char*>(pbkdf2Key.data())
                              )){
            qDebug() << "Key Created";
            qDebug() << "  Elapsed time :" << timer.elapsed() << "ms";
            qDebug() << "  Hmac         :" << pbkdf2Key.mid(0, 32).toBase64();
            if (this->hmac == pbkdf2Key.mid(0, 32)){
                this->aesKey = pbkdf2Key.mid(32, 32);
                return true;
            }else{
                qDebug() << "wrong password";
                return false;
            }
        }else{
            qDebug() << "error Failed to create key";
            return false;
        }
    }

    void UpdateIndex()
    {
        qDebug() << "empty function";
        // QElapsedTimer timer;
        // timer.start();
        // qDebug() << "[VAULT] indexing...";
        // QApplication::processEvents();
        // cipherIndex.clear();
        // plainIndex.clear();
        // for (int i = 0; i < files.size(); i++){
        //     if (files[i].state == file_t::CipherData)
        //         cipherIndex.push_back(i);
        //     else
        //         plainIndex.push_back(i);
        // }
        // qDebug() << "[VAULT] Indexing done";
        // qDebug() << "  Elapsed time    :" << timer.elapsed() << "ms";
        // qDebug() << "  Encrypted files :" << cipherIndex.size();
        // qDebug() << "  Decrypted files :" << plainIndex.size();
        // qDebug() << "  Total           :" << files.size();
    }

    void LoadFiles()
    {
        QElapsedTimer timer;
        timer.start();
        if (directory.exists()){
            try{
                files.clear();
                std::wstring root = directory.path().toStdWString();
                for (auto it = fs::recursive_directory_iterator(root); it != fs::recursive_directory_iterator(); ++it){
                    if (it->is_directory() && it->path().filename() == L".vault"){
                        it.disable_recursion_pending();
                        continue;
                    }

                    if (it->is_regular_file()){
                        QFileInfo qinfo = QFileInfo(it->path());
                        FileMetadata* file = new FileMetadata;

                        file->path.displayPath = qinfo.fileName();
                        file->path.absolutepath = qinfo.absoluteFilePath();
                        file->path.relativePath = directory.relativeFilePath(qinfo.absoluteFilePath());
                        file->path.state = (qinfo.fileName().endsWith(".enc", Qt::CaseInsensitive) ? FileMetadata::CipherData : FileMetadata::PlainData);
                        if (file->path.state == FileMetadata::CipherData){
                            file->path.displayPath = file->path.displayPath.left(file->path.displayPath.size() - 4);
                        }
                        files.push_back(file);
                    }
                }
            } catch(std::exception& e){
                qDebug() << e.what();
                return;
            }

            std::sort(files.begin(), files.end(), [](const FileMetadata* a, const FileMetadata* b){
                return a->path.relativePath < b->path.relativePath;
            });

            qDebug() << "[VAULT] Loading done";
            qDebug() << "  Elapsed time :" << timer.elapsed() << "ms";
            qDebug() << "  Files        :" << files.size();
            UpdateIndex();
            return;
        }else{
            qDebug() << "[VAULT] ERROR directory not exists";
            return;
        }
    }
};


#endif // VAULT_H
