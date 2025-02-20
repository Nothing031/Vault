#ifndef VAULT_H
#define VAULT_H

#include <QVector>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QException>
#include <QElapsedTimer>

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

    QVector<FILE_INFO> files;
    QVector<int> index_encrypted;
    QVector<int> index_decrypted;

    void UpdateIndex()
    {
        QElapsedTimer timer;
        timer.start();
        qDebug() << "[VAULT] indexing...";
        index_encrypted.clear();
        index_decrypted.clear();
        for (int i = 0; i < files.size(); i++){
            if (files[i].encrypted)
                index_encrypted.push_back(i);
            else
                index_decrypted.push_back(i);
        }
        qDebug() << "[VAULT] indexing done. elapsed time " << timer.elapsed() << "ms";
    }
    bool LoadFiles()
    {
        QElapsedTimer timer;
        timer.start();
        qDebug() << "[VAULT] loading files...";
        if (dir.exists()){
            files.clear();
            QVector<QFileInfo> fileInfoVec = dir.entryInfoList(QDir::Files, QDir::SortFlag::Name);
            for (const auto& fileInfo : fileInfoVec){
                FILE_INFO file = {fileInfo, dir.relativeFilePath(fileInfo) ,(fileInfo.path().endsWith(".enc") ? true : false)};
                files.push_back(file);
            }
            qDebug() << "[VAULT] loading done. elapsed time " << timer.elapsed() << "ms";
            qDebug() << "[VAULT] calling method UpdateIndex()";
            UpdateIndex();
        }else{
            qDebug() << "[VAULT] ERROR directory not exists";
            return false;
        }
    }
};


#endif // VAULT_H
