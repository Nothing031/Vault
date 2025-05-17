#include "Vault.hpp"

#include "src/Manifest.hpp"
#include "src/core/FileInfo.hpp"
#include <QMutexLocker>

Vault::Vault()
{
    directory   = {};
    files       = {};
    version     = FileInfo().header.version;
    globalSalt  = {};
    iteration   = ITERATION;
    hmac        = {};
    aesKey      = {};
}

Vault::Vault(const Vault &other)
{
    directory   = other.directory;
    files       = other.files;
    unlocked    = other.unlocked;
    version     = other.version;
    globalSalt  = other.globalSalt;
    iteration   = other.iteration;
    hmac        = other.hmac;
    aesKey      = other.aesKey;
}

Vault::~Vault()
{
    for (int i = 0; i < files.size(); i++)
        delete files[i];
}

Vault &Vault::operator=(const Vault &other)
{
    if (this == &other)
        return *this;
    directory   = other.directory;
    files       = other.files;
    unlocked    = other.unlocked;
    version     = other.version;
    globalSalt  = other.globalSalt;
    iteration   = other.iteration;
    hmac        = other.hmac;
    aesKey      = other.aesKey;
    return *this;
}

Vault* Vault::Create(const QString &dir, const QString &password, const int &iteration)
{
    Q_UNUSED(iteration);

    Vault* vault = new Vault;
    vault->directory = QDir(dir);
    vault->iteration = ITERATION;

    vault->globalSalt.resize(FileInfo::Sizes::salt);
    RAND_bytes(reinterpret_cast<unsigned char*>(vault->globalSalt.data()), FileInfo::Sizes::salt);

    // create key;
    QByteArray pbkdf2Key;
    if (_CreateKey(password, vault->globalSalt, iteration, pbkdf2Key)){
        vault->hmac = pbkdf2Key.mid(0, 32);
    }else{
        return nullptr;
    }
    return vault;
}

Vault* Vault::Load(const QString &dir)
{
    Vault* vault = new Vault;
    QFile file(dir + "/.vault/vault.json");

    if (file.exists() && file.open(QFile::ReadOnly | QFile::ExistingOnly)){
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument jDoc = QJsonDocument::fromJson(data);
        QJsonObject jObj = jDoc.object();
        vault->version     = jObj["version"].toString(ENV_VERSION);
        vault->globalSalt  = QByteArray::fromBase64(jObj["globalSalt"].toString("").toUtf8());
        vault->hmac        = QByteArray::fromBase64(jObj["hmac"].toString("").toUtf8());
        vault->iteration   = jObj["iteration"].toInt(ENV_DEFAULT_ITERATION);
        return vault;
    }else{
        return nullptr;
    }
}

bool Vault::SaveDataFile()
{
    if (QDir().exists(directory.path() + "/.vault"))
        QDir().mkpath(directory.path() + "/.vault");

    QFile file(directory.path() + "/.vault/vault.json");

    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QJsonObject jObj;
        jObj["version"]     = ENV_VERSION;
        jObj["globalSalt"]  = QString(globalSalt.toBase64());
        jObj["hmac"]        = QString(hmac.toBase64());
        jObj["iteration"]   = iteration;
        QJsonDocument jDoc(jObj);
        file.write(jDoc.toJson());
        file.close();
        return true;
    }
    else
        return false;
}

void Vault::VerifyPassword(const QString &password){
    QElapsedTimer timer;
    timer.start();
    qDebug() << "Creating key...";

    QByteArray pbkdf2Key;
    if (_CreateKey(password, globalSalt, iteration, pbkdf2Key) && pbkdf2Key.mid(0, 32) == hmac)
        goto success;
    else
        goto failed;


failed:
    unlocked = false;
    qDebug() << "wrong password";
    return;

success:
    aesKey = pbkdf2Key.mid(32, 32);
    unlocked = true;
    qDebug() << "Key Created";
    qDebug() << "  Elapsed time :" << timer.elapsed() << "ms";
    qDebug() << "  Hmac         :" << pbkdf2Key.mid(0, 32).toBase64();
    return;
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

                file->path.displayPath = qinfo.fileName();
                file->path.absolutepath = qinfo.absoluteFilePath();
                file->path.relativePath = directory.relativeFilePath(qinfo.absoluteFilePath());
                file->state = (qinfo.fileName().endsWith(".enc", Qt::CaseInsensitive) ? FileInfo::CipherData : FileInfo::PlainData);
                if (file->state == FileInfo::CipherData){
                    file->path.displayPath = file->path.displayPath.left(file->path.displayPath.size() - 4);
                }
                files.push_back(file);
            }
        }
    } catch(std::exception& e){
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

bool Vault::_CreateKey(const QString &str, const QByteArray &salt, const int &itr, QByteArray &out)
{
    out.resize(64);
    int result = PKCS5_PBKDF2_HMAC(reinterpret_cast<const char*>(str.constData()),
                                   str.size(),
                                   reinterpret_cast<const unsigned char*>(salt.constData()),
                                   salt.size(),
                                   itr,
                                   EVP_sha256(),
                                   64,
                                   reinterpret_cast<unsigned char*>(out.data())
                                   );
    if (result == 0)
        qDebug() << "Error failed to create key";
    return result;
}

