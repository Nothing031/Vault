#ifndef ENVIROMENT_HPP
#define ENVIROMENT_HPP

#include <QWidget>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "vault.hpp"

#define ENV_VAULT_PATH "vault.json"
#define ENV_SETTING_PATH "setting.json"
#define ENV_BACKUPDIR_LENGTH 8

#define KEY_VAULTS "Vault"
#define KEY_PASSWORD "Password"
#define KEY_DIRECTORY "Directory"


class Enviroment{
public:
    static Enviroment& GetInstance(){
        static Enviroment instance;
        return instance;
    }

    Enviroment(){

    };

    void Init(QComboBox* box){
        if (box){
            Load();
            box->blockSignals(true);
            for (int i = 0; i < vaults.size(); i++){
                vaults[i].id = i;
                box->addItem(vaults[i].name, QVariant::fromValue(static_cast<void*>(&vaults[i])));
            }
            box->setCurrentIndex(-1);
            box->blockSignals(false);
        }else{
            qDebug() << "Failed to initialize enviroment";
        }
    }

    Vault* getVault(int index){
        if (index < 0 || index >= vaults.size()){
            return nullptr;
        }
        return &vaults[index];
    }

    void AddNewVault(const Vault& vault, QComboBox* box){
        if (box){
            // add
            box->blockSignals(true);
            box->clear();
            vaults.push_back(vault);
            for (int i = 0; i < vaults.size(); i++){
                vaults[i].id = i;
                box->addItem(vaults[i].name, QVariant::fromValue(static_cast<void*>(&vaults[i])));
            }
            box->setCurrentIndex(-1);
            box->blockSignals(false);
            qDebug() << "Vault Added, vault : " << vault.directory.path();
        }else{
            qDebug() << "[ERROR] Failed to add vault";
            return;
        }
        Save();
    }

    void DetachVault(Vault* pvault, QComboBox* box){
        if (box && pvault){
            // find index
            int index;
            for (index = 0; index < vaults.size();){
                if (pvault == &vaults[index]){
                    break;
                }else if (++index == vaults.size()){
                    qDebug() << "[ERROR] Failed to detach vault : " << pvault->directory.path();
                    return;
                }
            }
            // remove
            box->blockSignals(true);
            box->clear();
            vaults.remove(index);
            for (int i = 0; i < vaults.size(); i++){
                vaults[i].id = i;
                box->addItem(vaults[i].name, QVariant::fromValue(static_cast<void*>(&vaults[i])));
            }
            box->setCurrentIndex(-1);
            box->blockSignals(false);
            qDebug() << "Vault Detached";
        }else{
            qDebug() << "[ERROR] parameters are nullptr";
            return;
        }
        Save();
    }

    void Save(){
        QJsonObject jObj;
        QJsonArray jVaults;

        for (auto& vault : std::as_const(vaults)){
            QJsonObject jVault;
            jVault[KEY_DIRECTORY] = vault.directory.path();
            jVault[KEY_PASSWORD] = vault.password;
            jVaults.push_back(jVault);
        }
        jObj[KEY_VAULTS] = jVaults;

        QJsonDocument jDoc(jObj);

        QFile file(ENV_VAULT_PATH);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)){
            qDebug() << "[ERROR] Json Save Failure";
            return;
        }

        file.write(jDoc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "Json Saved";
    }

    void Load(){
        QFile file(ENV_VAULT_PATH);

        if (!file.exists() || !file.open(QFile::ReadOnly)){
            file.open(QFile::WriteOnly);
            QJsonObject jObj;
            jObj[KEY_VAULTS] = QJsonArray();
            QJsonDocument jDoc(jObj);
            file.write(jDoc.toJson());
            file.close();
            qDebug() << "Json Created";
            return;
        }

        QByteArray data = file.readAll();
        QJsonDocument jDoc = QJsonDocument::fromJson(data);
        QJsonObject jObj = jDoc.object();
        QJsonArray jVaults = jObj[KEY_VAULTS].toArray();
        for (auto& jVault : std::as_const(jVaults)){
            QJsonObject jVaultObj = jVault.toObject();
            QString directory = jVaultObj[KEY_DIRECTORY].toString();
            QString password = jVaultObj[KEY_PASSWORD].toString();
            vaults.emplaceBack(directory, password, ENV_BACKUPDIR_LENGTH);
        }
        file.close();
        qDebug() << "Json Loaded";
    }

private:
    QVector<Vault> vaults;

};


#endif // ENVIROMENT_HPP
