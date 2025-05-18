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

class Vault
{
public:
    QDir                directory;
    QMutex              mutex;
    QVector<FileInfo*>  files;

    QByteArray          aesKey;
    bool                unlocked = false;

    FileInfo::FileHeader header;


    Vault();
};

