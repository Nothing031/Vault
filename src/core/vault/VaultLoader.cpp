#include "VaultLoader.hpp"

#include <QFile>
#include <QJsonObject>

VaultLoader VaultLoader::instance;

VaultLoader::VaultLoader()
{

}

VaultLoader &VaultLoader::GetInstance()
{
    return instance;
}

void VaultLoader::LoadVault(Vault* vault)
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
        vault->formatVersion = jObj["FormatVersion"].toString("UNKNOWN");
        vault->aes = AES256Settings::FromJsonObject(aesObj);
        vault->excludeChecker = ExcludeChecker::FromJsonObject(excludeObj);

        lastError = LoadSucceeded;
        emit onEvent(LoadSucceeded);
        qDebug() << "Load Succeeded";
    }else{
        lastError = LoadFailed;
        emit onEvent(LoadFailed);
        qDebug() << "Load Failed";
    }
}

void VaultLoader::SaveVault(Vault* vault)
{
    if (QDir().exists(vault->directory.path() + "/.vault"))
        QDir().mkpath(vault->directory.path() + "/.vault");

    QFile file(vault->directory.path() + "/.vault/vault.json");

    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QJsonObject jObj;
        jObj["AppVersion"] = vault->appVersion;
        jObj["FormatVersion"] = vault->formatVersion;
        jObj["EncryptionSettings"] = vault->aes.ToJsonObject();
        jObj["ExcludeSettings"] = vault->excludeChecker.ToJsonObject();

        QJsonDocument jDoc(jObj);
        file.write(jDoc.toJson());
        file.close();
        lastError = SaveSucceeded;
        emit onEvent(SaveSucceeded);
        qDebug() << "Save Succeeded";
    }else{
        lastError = SaveFailed;
        emit onEvent(SaveFailed);
        qDebug() << "Save Failed";
    }
}

VaultLoader::Event VaultLoader::GetLastError()
{
    return lastError;
}
