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
#include "src/core/FileInfo.hpp"
#include "AES256Settings.hpp"
#include "ExcludeChecker.hpp"

#define EXTENSION ".enc"

class Vault
{
public:
    QString             appVersion;

    QDir                directory;
    QMutex              mutex;
    QVector<FileInfo*>  files;

    ExcludeChecker      excludeChecker;
    AES256Settings      aesSettings;

    Vault();
    ~Vault();

    bool CheckPassword(const QString& password);
    void LoadFiles();
};

