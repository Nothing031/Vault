#pragma once

#include <QDir>
#include <QFile>
#include <QMutex>
#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QException>
#include <QDirListing>
#include <QApplication>
#include <QDirIterator>
#include <QElapsedTimer>

#include <memory>

#include "src/core/fileinfo/FileInfo.hpp"
#include "src/core/cryptography/AES256Settings.hpp"
#include "src/Manifest.hpp"


class Vault
{
public:
    void                *owner = nullptr;
    QString             appVersion = APP_VERSION;
    QString             saveFormatVersion = SAVE_FORMAT_VERSION;

    QMutex              mutex;
    QDir                directory;
    QVector<std::shared_ptr<FileInfo>> files;

    AES256Settings      aes;

    Vault();
    ~Vault();

    void LoadFiles();
};

Q_DECLARE_METATYPE(std::shared_ptr<Vault>)
