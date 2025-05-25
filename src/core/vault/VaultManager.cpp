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


void VaultManager::DetachVault(Vault* vault)
{
    for (int i = 0; i < vaults.size(); i++){
        if (vaults[i] == vault){
            vaults.remove(i, 1);
            delete vault;
            emit onVaultRemoved(vault);
        }
    }
}

void VaultManager::CreateVault(const bool& aesEnabled, const QString &dir, const QString &password)
{
    Vault* vault = new Vault;
    if (aesEnabled){
        vault->aesSettings.SetEnabled(true);
        QByteArray salt(FileInfo::Sizes::salt, 0);
        RAND_bytes((unsigned char*)salt.data(), FileInfo::Sizes::salt);
        vault->aesSettings.SetGlobalSalt(salt);
        QByteArray key = Cryptography::GenerateKey(password, vault->aesSettings.GlobalSalt(), vault->aesSettings.Iteration());
        vault->aesSettings.SetHmac(key.mid(0, 32));
    }else{
        vault->aesSettings.SetEnabled(false);
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
    emit onVaultAdded(vault);
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
