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
#include "src/core/fileinfo/FileInfo.hpp"
#include "AES256Settings.hpp"
#include "ExcludeChecker.hpp"
#include "src/Manifest.hpp"

class Vault
{
public:
    void                *owner = nullptr;
    QString             appVersion = APPVERSION;
    QString             formatVersion = FORMATVERSION;

    QDir                directory;
    QMutex              mutex;
    QVector<FileInfo*>  files;

    ExcludeChecker      excludeChecker;
    AES256Settings      aes;

    Vault();
    ~Vault();

    void LoadFiles();
};

