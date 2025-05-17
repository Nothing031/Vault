#pragma once


#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "vault.hpp"

class VaultManager : public QObject{
    Q_OBJECT

public:
    VaultManager(){

    }

    ~VaultManager(){
        for (int i = 0; i < vaults.size(); ++i){
            delete vaults[i];
        }
    }

    static VaultManager& GetInstance(){
        static VaultManager instance;
        return instance;
    }

    Vault* GetVault(int index){
        if (index < 0 || index >= vaults.size())
            return nullptr;

        return vaults[index];
    }

public slots:
    void DetachVault(int index){
        if (index < 0 || index >= vaults.size()){
            qDebug() << "Error failed to detach vault";
            return;
        }

        Vault* pVault = vaults[index];
        vaults.remove(index, 1);
        delete pVault;
    }

    void AttachVault(Vault* pVault){
        vaults.append(pVault);
        emit AttachVault(pVault);
    }

signals:
    void onDetachVault(int index);

    void onAttachVault(Vault* pVault);

private:
    void LoadVaults(){
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
            Vault* vault = Vault::Load(vaultDir);
            if (vault){
                vaults.append(vault);
                emit onAttachVault(vault);
            }
            else{
                qDebug() << "Failed to load vault : " << vaultDir;
                continue;
            }
        }
    }

    void SaveVaults(){
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

    QVector<Vault*> vaults;
};

