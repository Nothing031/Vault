#include "VaultManager.hpp"

#include "src/core/cryptography/Cryptography.hpp"
#include "VaultLoader.hpp"
#include "src/core/fileinfo/FileHeader.hpp"

#include <openssl/rand.h>

VaultManager VaultManager::instance;

VaultManager::VaultManager()
{

}

VaultManager::~VaultManager()
{
    SaveData();
}

VaultManager& VaultManager::GetInstance()
{
    return instance;
}

std::shared_ptr<Vault> VaultManager::GetVault(int index)
{
    if (0 > index || index >= vaults.size())
        return vaults[index];
    else
        return nullptr;
}

void VaultManager::CreateVault(const bool& aesEnabled, const QString &dir, const QString &password)
{
    std::shared_ptr<Vault> vault = std::make_shared<Vault>();
    vault->owner = this;
    vault->directory = QDir(dir);

    if (aesEnabled){
        vault->aes.SetEnabled(true);
        QByteArray salt(FileHeader::Sizes::salt, 0);
        RAND_bytes((unsigned char*)salt.data(), FileHeader::Sizes::salt);
        vault->aes.SetGlobalSalt(salt);
        QByteArray key = Cryptography::GenerateKey(password, vault->aes.GlobalSalt(), vault->aes.Iteration());
        vault->aes.SetHmac(key.mid(0, 32));
    }else{
        vault->aes.SetEnabled(false);
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
    if (error == VaultLoader::FAILED){
        throw std::exception("Failed to create vault");
    }

    vaults.append(vault);
    emit onVaultAdded(vault);
}

void VaultManager::DetachVault(std::shared_ptr<Vault> vault)
{
    if (vault->owner != this){
        qDebug() << "owner has been changed Failed to delete";
        return;
    }

    for (int i = 0; i < vaults.size(); i++){
        if (vaults[i] == vault){
            vaults.remove(i, 1);
            emit onVaultRemoved(vault);
        }
    }
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

        std::shared_ptr<Vault> vault = std::make_shared<Vault>();
        vault->owner = this;
        vault->directory = vaultDir;
        loader.LoadVault(vault);
        VaultLoader::Event error = loader.GetLastError();
        if (error == VaultLoader::FAILED){
            qDebug() << "Failed to laod vault : " << vault->directory;
        }
        else{
            vaults.append(vault);
            emit onVaultAdded(vault);
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
