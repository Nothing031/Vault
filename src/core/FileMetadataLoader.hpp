#pragma once

#include <QObject>
#include <QTimer>
#include <QThread>


// #include "Vault.hpp"
// #include "FileInfo.hpp"

// class FileMetadataLoader : public QObject{
//     Q_OBJECT
// public:
//     FileMetadataLoader(QObject* parent = nullptr)
//         : thread(new QThread)
//         , timer(new QTimer)
//     {
//         timer->moveToThread(thread);
//     }
//     ~FileMetadataLoader(){
//         Stop();
//         delete timer;
//         delete thread;
//     }

//     static FileMetadataLoader& GetInstance(){
//         static FileMetadataLoader instance;
//         return instance;
//     }

//     void SetVault(Vault* vault = nullptr){
//         Stop();
//         this->vault = vault;
//         index = 0;
//     }

//     void Start(){
//         if (!vault){
//             qDebug() << "FileMetadataLoader::Start() error, vault is nullptr";
//             return;
//         }

//         if (!timer->isActive()){
//             timer->start(10);
//             qDebug() << "FileMetadataLoader Started";
//         }
//     }

//     void Stop(){
//         if (timer->isActive()){
//             timer->stop();
//             qDebug() << "FileMetadataLoader stopped";
//         }
//     }

//     bool IsRunning(){
//         return timer->isActive();
//     }

// private slots:
//     void LoadMetadata(){
//         QFile file;
//         QByteArray data;
//         for (; index < vault->files.size(); index++){
//             file.setFileName(vault->files.at(index)->path.absolutepath);
//             if (!file.open(QFile::ReadOnly) || file.size() < FileInfo::Header::sizes::total)
//                 continue;

//             data = file.read(FileInfo::Header::sizes::total);
//             file.close();
//             bool result = vault->files[index]->header.Set(data);
//             if (!result)
//                 qDebug() << "Failed to load header;";
//         }
//     }

// private:

//     QThread *thread;
//     QTimer *timer;

//     int index = 0;
//     Vault *vault = nullptr;
// };

