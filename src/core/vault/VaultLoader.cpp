#include "VaultLoader.hpp"

#include <QFile>
#include <QJsonObject>
#include <memory>

VaultLoader VaultLoader::instance;

VaultLoader::VaultLoader()
{

}

VaultLoader &VaultLoader::GetInstance()
{
    return instance;
}

void VaultLoader::LoadVault(std::shared_ptr<Vault> vault)
{
    QFile file(vault->directory.path() + "/.vault/vault.json");

    if (file.exists() && file.open(QFile::ReadOnly | QFile::ExistingOnly)){
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument jDoc = QJsonDocument::fromJson(data);
        QJsonObject jObj = jDoc.object();

        QJsonObject excludeObj = jObj["ExcludeSettings"].toObject();
        QJsonObject aesObj = jObj["EncryptionSettings"].toObject();
        vault->appVersion = jObj["AppVersion"].toString("UNKNOWN");
        vault->saveFormatVersion = jObj["FormatVersion"].toString("UNKNOWN");
        vault->aes = AES256Settings::FromJsonObject(aesObj);

        lastError = CLEAN;
        emit onEvent(CLEAN);
        qDebug() << "Load Succeeded";
    }else{
        lastError = FAILED;
        emit onEvent(FAILED);
        qDebug() << "Load Failed";
    }
}

void VaultLoader::SaveVault(std::shared_ptr<Vault> vault)
{
    if (QDir().exists(vault->directory.path() + "/.vault"))
        QDir().mkpath(vault->directory.path() + "/.vault");

    QFile file(vault->directory.path() + "/.vault/vault.json");

    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QJsonObject jObj;
        jObj["AppVersion"] = vault->appVersion;
        jObj["FormatVersion"] = vault->saveFormatVersion;
        jObj["EncryptionSettings"] = vault->aes.ToJsonObject();

        QJsonDocument jDoc(jObj);
        file.write(jDoc.toJson());
        file.close();
        lastError = CLEAN;
        emit onEvent(CLEAN);
        qDebug() << "Save Succeeded";
    }else{
        lastError = FAILED;
        emit onEvent(FAILED);
        qDebug() << "Save Failed";
    }
}

VaultLoader::Event VaultLoader::GetLastError()
{
    return lastError;
}
