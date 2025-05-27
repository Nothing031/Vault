#pragma once

#include <QString>
#include <QByteArray>
#include <QDebug>

#include "FileInfoHeader.hpp"

#define FORMAT_VERSION "0002"
#define SIGNATURE "VAULT BY NOTHING031"
#define ITERATION 100000
#define EXTENSION ".enc"

struct FileInfo{
    enum State{
        PlainData,
        CipherData
    };
    struct FilePath{
        QString absolutepath;
        QString relativePath;
        QString displayPath;
    };


    bool integrity = true;
    bool isHeaderMatch = true;
    State state = PlainData;
    FileInfoHeader header;
    FilePath path;
};
