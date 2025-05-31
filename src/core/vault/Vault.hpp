#pragma once

// qtl
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

//stl
#include <memory>

// mine
#include "src/core/fileinfo/FileInfo.hpp"
#include "AES256Settings.hpp"
#include "ExcludeChecker.hpp"
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
