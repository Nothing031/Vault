#include "VaultManager.hpp"

#include "src/core/cryptography/Cryptography.hpp"
#include "VaultLoader.hpp"

VaultManager VaultManager::instance;

VaultManager::VaultManager()
{

}

VaultManager::~VaultManager()
{
    SaveData();
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

void VaultManager::DetachVault(int index)
{
    if (0 > index || index >= vaults.size())
        throw std::exception("out of range");

    Vault* vault = vaults[index];
    vaults.remove(index, 1);
    delete vault;

    emit onDetachVault(index);
}

void VaultManager::CreateVault(const bool& aesEnabled, const QString &dir, const QString &password)
{
    Vault* vault = new Vault;
    if (aesEnabled){
        vault->AES256Enabled = true;
        vault->header.salt.resize(FileInfo::Sizes::salt);
        RAND_bytes((unsigned char*)vault->header.salt.data(), FileInfo::Sizes::salt);
        QByteArray key = Cryptography::GenerateKey(password, vault->header.salt, vault->header.iteration);
        vault->header.hmac = key.mid(0, 32);
    }else{
        vault->AES256Enabled = false;
    }

    // create directory
    vault->directory = dir;
    if (!vault->directory.exists())
        vault->directory.mkpath(vault->directory.path());
    if (!vault->directory.exists(".vault"))
        vault->directory.mkpath(vault->directory.path() + "/.vault");

    VaultLoader& loader = VaultLoader::GetInstance();
    loader.SaveVault(vault);
    VaultLoader::Event error = loader.GetLastError();
    if (error == VaultLoader::SaveFailed){
        throw std::exception("Failed to create vault");
    }

    vaults.append(vault);
    emit onAttachVault(vault);
}

void VaultManager::LoadData()
{
    QFile file("vaults.json");
    if (!file.exists() || !file.open(QFile::ReadOnly | QFile::ExistingOnly))
        return;

    QByteArray data = file.readAll();
    QJsonDocument jDoc = QJsonDocument::fromJson(data);
    QJsonObject jObj = jDoc.object();

    QJsonArray vaultArr = jObj["vaults"].toArray();
    VaultLoader& loader = VaultLoader::GetInstance();
    for (auto& vaultVal : std::as_const(vaultArr)){
        auto vaultObj = vaultVal.toObject();
        QString vaultDir = vaultObj["directory"].toString("");

        Vault* vault = new Vault;
        vault->directory = vaultDir;
        loader.LoadVault(vault);
        VaultLoader::Event error = loader.GetLastError();
        if (error == VaultLoader::LoadFailed){
            qDebug() << "Failed to laod vault : " << vault->directory;
        }
        else{
            vaults.append(vault);
            emit onAttachVault(vault);
        }
    }
}

void VaultManager::SaveData()
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
