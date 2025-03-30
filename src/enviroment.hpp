#ifndef ENVIROMENT_HPP
#define ENVIROMENT_HPP

#include <QWidget>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "vault.hpp"

#define ENV_VAULT_PORTABLE_PATH "vault.json"
#define ENV_VAULT_LOCAL_PATH "vaults.json"
#define ENV_SETTING_PATH "settings.json"

#define KEY_VAULTS_LOCAL "Vaults"
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
            LoadLocalVaults();
            box->blockSignals(true);
            for (int i = 0; i < vaults.size(); i++){
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
                box->addItem(vaults[i].name, QVariant::fromValue(static_cast<void*>(&vaults[i])));
            }
            box->setCurrentIndex(-1);
            box->blockSignals(false);
            qDebug() << "Vault Added, vault : " << vault.directory.path();
        }else{
            qDebug() << "[ERROR] Failed to add vault";
            return;
        }
        SaveLocalVaults();
    }

    void DetachVault(Vault* pVault, QComboBox* box){
        if (box && pVault){
            // find index
            int index;
            for (index = 0; index < vaults.size();){
                if (pVault == &vaults[index]){
                    break;
                }else if (++index == vaults.size()){
                    qDebug() << "[ERROR] Failed to detach vault : " << pVault->directory.path();
                    return;
                }
            }
            // remove
            box->blockSignals(true);
            box->clear();
            vaults.remove(index);
            for (int i = 0; i < vaults.size(); i++){
                box->addItem(vaults[i].name, QVariant::fromValue(static_cast<void*>(&vaults[i])));
            }
            box->setCurrentIndex(-1);
            box->blockSignals(false);
            qDebug() << "Vault Detached";
        }else{
            qDebug() << "[ERROR] parameters are nullptr";
            return;
        }
        SaveLocalVaults();
    }

    void SavePortable(const Vault& vault){
        QString savePath = vault.directory.path() + "/" + ENV_VAULT_PORTABLE_PATH;
        QJsonObject jObj;
        jObj[KEY_DIRECTORY] = vault.directory.path();
        jObj[KEY_PASSWORD] = vault.sha256Password;
        QJsonDocument jDoc(jObj);
        QFile file(savePath);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)){
            qDebug() << "[ERROR] Json Save Failure";
            return;
        }
        file.write(jDoc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "json saved";
    }

    Vault LoadPortable(const QString& path){
        QFile file(path);

        if (!file.exists() || !file.open(QFile::ReadOnly)){
            qDebug() << "failed to read json";
            return Vault();
        }

        QByteArray data = file.readAll();
        QJsonDocument jDoc = QJsonDocument::fromJson(data);
        QJsonObject jObj = jDoc.object();
        QString directory = jObj[KEY_DIRECTORY].toString();
        QString password  = jObj[KEY_PASSWORD].toString();
        return Vault(directory, password, Mode::Portable);
    }

    void SaveLocalVaults(){
        QJsonObject jObj;
        QJsonArray jVaults;

        for (auto& vault : std::as_const(vaults)){
            QJsonObject jVault;
            jVault[KEY_DIRECTORY] = vault.directory.path();
            jVault[KEY_PASSWORD] = vault.sha256Password;
            jVaults.push_back(jVault);
        }
        jObj[KEY_VAULTS_LOCAL] = jVaults;

        QJsonDocument jDoc(jObj);

        QFile file(ENV_VAULT_LOCAL_PATH);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)){
            qDebug() << "[ERROR] Json Save Failure";
            return;
        }

        file.write(jDoc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "Json Saved";
    }

    void LoadLocalVaults(){
        QFile file(ENV_VAULT_LOCAL_PATH);

        if (!file.exists() || !file.open(QFile::ReadOnly)){
            file.open(QFile::WriteOnly);
            QJsonObject jObj;
            jObj[KEY_VAULTS_LOCAL] = QJsonArray();
            QJsonDocument jDoc(jObj);
            file.write(jDoc.toJson());
            file.close();
            qDebug() << "Json Created";
            return;
        }

        QByteArray data = file.readAll();
        QJsonDocument jDoc = QJsonDocument::fromJson(data);
        QJsonObject jObj = jDoc.object();
        QJsonArray jVaults = jObj[KEY_VAULTS_LOCAL].toArray();
        for (auto& jVault : std::as_const(jVaults)){
            QJsonObject jVaultObj = jVault.toObject();
            QString directory = jVaultObj[KEY_DIRECTORY].toString();
            QString password = jVaultObj[KEY_PASSWORD].toString();
            vaults.emplaceBack(directory, password);
        }
        file.close();
        qDebug() << "Json Loaded";
    }

private:
    QVector<Vault> vaults;

};


#endif // ENVIROMENT_HPP
