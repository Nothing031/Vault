#include "FileInfoLoader.hpp"

#include <QFile>

FileInfoLoader FileInfoLoader::instance;

FileInfoLoader::FileInfoLoader()
{

}

FileInfoLoader::~FileInfoLoader()
{

}

FileInfoLoader &FileInfoLoader::GetInstance()
{
    return instance;
}

void FileInfoLoader::LoadInfoSingle(QString path, void* caller)
{
    QFile file(path);
    if (file.exists() && file.open(QFile::ReadOnly | QFile::ExistingOnly)){
        if (file.size() >= FileHeader::Sizes::total){
            QByteArray data = file.read(FileHeader::Sizes::total);
            FileHeader header;
            auto state = header.Deserialize(data);
            emit onSingleInfoLoaded(state, header, caller);
        }
    }
    FileHeader header;
    emit onSingleInfoLoaded(FileInfo::UNKNOWN_NONE, header, caller);
}









