#pragma once
#include <QObject>

#include "FileInfo.hpp"

class FileInfoLoader : public QObject
{
    Q_OBJECT
public:
    explicit FileInfoLoader(QObject *parent = nullptr);
    ~FileInfoLoader();

public slots:
    void LoadInfoSingle(QString path);


signals:
    void onInfoLoaded(FileInfo info);


private:
    static FileInfoLoader instance;
};

