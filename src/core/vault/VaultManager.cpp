#include "VaultManager.hpp"

#include "src/core/cryptography/Cryptography.hpp"


VaultManager VaultManager::instance;

VaultManager::VaultManager(){}

VaultManager::~VaultManager()
{
    SaveVaultManagerData();
    for (auto& vault : vaults) { delete vault; }
}

VaultManager& VaultManager::GetInstance()
{
    return instance;
}

Vault* VaultManager::GetVault(int index)
{
    if (0 > index || index >= vaults.size())
        return vaults[index];
    else
        return nullptr;
}

bool VaultManager::SaveVaultData(Vault* vault)
{
    if (QDir().exists(vault->directory.path() + "/.vault"))
        QDir().mkpath(vault->directory.path() + "/.vault");

    QFile file(vault->directory.path() + "/.vault/vault.json");

    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QJsonObject jObj;
        jObj["version"]     = QString(vault->header.version.toBase64());
        jObj["globalSalt"]  = QString(vault->header.salt.toBase64());
        jObj["hmac"]        = QString(vault->header.hmac.toBase64());
        jObj["iteration"]   = vault->header.iteration;
        QJsonDocument jDoc(jObj);
        file.write(jDoc.toJson());
        file.close();
        return true;
    }else{
        return false;
    }
}

bool VaultManager::LoadVaultData(Vault* vault)
{
    QFile file(vault->directory.path() + "/.vault/vault.json");

    if (file.exists() && file.open(QFile::ReadOnly | QFile::ExistingOnly)){
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument jDoc = QJsonDocument::fromJson(data);
        QJsonObject jObj = jDoc.object();
        vault->header.version   = QByteArray::fromBase64(jObj["version"].toString(VERSION).toUtf8());
        vault->header.salt      = QByteArray::fromBase64(jObj["globalSalt"].toString("").toUtf8());
        vault->header.hmac      = QByteArray::fromBase64(jObj["hmac"].toString("").toUtf8());
        vault->header.iteration = jObj["iteration"].toInt(ITERATION);
        return true;
    }else{
        return false;
    }
}

void VaultManager::DetachVault(int index)
{
    if (0 > index || index >= vaults.size())
        throw std::exception("out of range");

    Vault* vault = vaults[index];
    vaults.remove(index, 1);
    delete vault;

    emit onDetachVault(index);
}

void VaultManager::CreateVault(const QString &dir, const QString &password)
{
    Vault* vault = new Vault;
    vault->header.salt.resize(FileInfo::Sizes::salt);
    RAND_bytes((unsigned char*)vault->header.salt.data(), FileInfo::Sizes::salt);
    QByteArray key = Cryptography::GenerateKey(password, vault->header.salt, vault->header.iteration);
    vault->header.hmac = key.mid(0, 32);

    // create directory
    vault->directory = dir;
    if (!vault->directory.exists())
        vault->directory.mkpath(vault->directory.path());
    if (!vault->directory.exists(".vault"))
        vault->directory.mkpath(vault->directory.path() + "/.vault");

    bool success = SaveVaultData(vault);
    if (success){
        vaults.append(vault);
        emit onAttachVault(vault);
    }else{
        throw std::exception("Failed to create vault");
    }
}

void VaultManager::LoadVaultManagerData()
{
    QFile file("vaults.json");
    if (!file.exists() || !file.open(QFile::ReadOnly | QFile::ExistingOnly))
        return;

    QByteArray data = file.readAll();
    QJsonDocument jDoc = QJsonDocument::fromJson(data);
    QJsonObject jObj = jDoc.object();

    QJsonArray vaultArr = jObj["vaults"].toArray();
    for (auto& vaultVal : std::as_const(vaultArr)){
        auto vaultObj = vaultVal.toObject();
        QString vaultDir = vaultObj["directory"].toString("");

        Vault* vault = new Vault;
        vault->directory = vaultDir;
        bool success = LoadVaultData(vault);

        if (success){
            vaults.append(vault);
            emit onAttachVault(vault);
        }
        else{
            qDebug() << "Failed to load vault : " << vaultDir;
            continue;
        }
    }
}

void VaultManager::SaveVaultManagerData()
{
    QFile file("vaults.json");
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
        return;

    QJsonObject jObj;

    // vaults
    QJsonArray vaultArr;
    for (auto& vault : std::as_const(vaults)){
        QJsonObject vaultObj;
        vaultObj["directory"] = vault->directory.path();
        vaultArr.append(vaultObj);
    }
    jObj["vaults"] = vaultArr;

    QJsonDocument jDoc(jObj);
    file.write(jDoc.toJson());
    file.close();
}
