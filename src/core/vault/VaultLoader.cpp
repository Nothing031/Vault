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



        vault->header.version   = jObj["version"].toString(FORMAT_VERSION).toUtf8();
        vault->header.salt      = QByteArray::fromBase64(jObj["globalSalt"].toString("").toUtf8());
        vault->header.hmac      = QByteArray::fromBase64(jObj["hmac"].toString("").toUtf8());
        vault->header.iteration = jObj["iteration"].toInt(ITERATION);
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
        jObj["version"]     = QString(vault->header.version);
        jObj["globalSalt"]  = QString(vault->header.salt.toBase64());
        jObj["hmac"]        = QString(vault->header.hmac.toBase64());
        jObj["iteration"]   = vault->header.iteration;
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
