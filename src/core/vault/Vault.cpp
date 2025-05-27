#include "Vault.hpp"


#include <filesystem>

#include "src/core/cryptography/Cryptography.hpp"

namespace fs = std::filesystem;

Vault::Vault()
{

}

Vault::~Vault()
{
    for (auto& file : files){ delete file; }
}

void Vault::LoadFiles()
{
    QElapsedTimer timer;
    timer.start();
    if (!directory.exists()){
        qDebug() << "[VAULT] ERROR directory not exists";
        return;
    }

    QMutexLocker<QMutex> lock(&mutex);

    try{
        files.clear();
        std::wstring root = directory.path().toStdWString();
        for (auto it = fs::recursive_directory_iterator(root); it != fs::recursive_directory_iterator(); ++it){
            if (it->is_directory() && it->path().filename() == L".vault"){
                it.disable_recursion_pending();
                continue;
            }

            if (it->is_regular_file()){
                QFileInfo qinfo = QFileInfo(it->path());
                FileInfo* file = new FileInfo;

                file->path.absolutepath = qinfo.absoluteFilePath();
                file->path.relativePath = directory.relativeFilePath(qinfo.absoluteFilePath());
                file->path.displayPath = file->path.relativePath;
                file->state = (qinfo.fileName().endsWith(EXTENSION, Qt::CaseInsensitive) ? FileInfo::CipherData : FileInfo::PlainData);
                if (file->state == FileInfo::CipherData){
                    file->path.displayPath = file->path.displayPath.left(file->path.displayPath.size() - 4);
                }
                files.push_back(file);
            }
        }
    } catch(fs::filesystem_error& e){
        qDebug() << e.what();
        return;
    }

    std::sort(files.begin(), files.end(), [](const FileInfo* a, const FileInfo* b){
        return a->path.relativePath < b->path.relativePath;
    });

    qDebug() << "[VAULT] Loading done";
    qDebug() << "  Elapsed time :" << timer.elapsed() << "ms";
    qDebug() << "  Files        :" << files.size();
}

