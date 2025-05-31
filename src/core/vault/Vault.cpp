#include "Vault.hpp"


#include <filesystem>
namespace fs = std::filesystem;

Vault::Vault()
{

}

Vault::~Vault()
{

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
            // ignore reserved folder
            if (it->is_directory() && it->path().filename() == L".vault"){
                it.disable_recursion_pending();
                continue;
            }

            if (it->is_regular_file()){
                QFileInfo qinfo = QFileInfo(it->path());
                std::shared_ptr<FileInfo> file = std::make_shared<FileInfo>();

                file->path.absolutepath = qinfo.absoluteFilePath();
                file->path.displayPath = directory.relativeFilePath(qinfo.absoluteFilePath());
                file->state = (qinfo.fileName().endsWith(".enc", Qt::CaseInsensitive) ? FileInfo::CIPHER_GOOD : FileInfo::PLAIN_GOOD);
                if (file->state == FileInfo::CIPHER_GOOD){
                    file->path.displayPath = file->path.displayPath.left(file->path.displayPath.size() - 4);
                }
                files.append(file);
            }
        }
    } catch(fs::filesystem_error& e){
        qDebug() << e.what();
        return;
    }

    std::sort(files.begin(), files.end(), [](const std::shared_ptr<FileInfo>& a, const std::shared_ptr<FileInfo>& b){
        return a->path.displayPath < b->path.displayPath;
    });

    qDebug() << "[VAULT] vault loaded";
    qDebug() << "  Elapsed time :" << timer.elapsed() << "ms";
    qDebug() << "  Files        :" << files.size();
}

