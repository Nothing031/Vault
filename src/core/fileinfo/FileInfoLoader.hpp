#pragma once
#include <QObject>

#include "FileHeader.hpp"

class FileInfoLoader : public QObject
{
    Q_OBJECT
public:
    ~FileInfoLoader();
    static FileInfoLoader& GetInstance();

public:
    void LoadInfoSingle(QString path, void* caller);

signals:
    void onSingleInfoLoaded(FileInfo::State state, FileHeader info, void* caller);

private:
    FileInfoLoader();
    static FileInfoLoader instance;
};

