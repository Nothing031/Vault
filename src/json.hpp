#ifndef JSON_H
#define JSON_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QByteArray>
#include <QFile>
#include <QString>
#include <QVector>
#include <QDir>
#include <QFile>

#include "src/vault.hpp"

class Json{
private:
    QString jsonPath = "vault.json";

public:
    QVector<Vault> LoadVaultJson(){
        QFile file(jsonPath);
        if (!file.exists()){
            file.open(QFile::WriteOnly);
            QJsonObject jObj;
            jObj["Vaults"] = QJsonArray();
            QJsonDocument doc(jObj);
            file.write(doc.toJson());
            file.close();
            qDebug() << "[JSON] file not exists";
            return QVector<Vault>();
        }
        if (!file.open(QFile::ReadOnly)){
            qDebug() << "[JSON] failed to read file";
            return QVector<Vault>();
        }
        QByteArray data = file.readAll();
        QJsonDocument jDoc = QJsonDocument::fromJson(data);
        QJsonObject jObj = jDoc.object();
        QVector<Vault> vaults;
        QJsonArray jVaults = jObj["Vaults"].toArray();
        for (const auto& jVault : jVaults){
            Vault vault;
            QJsonObject jVaultObj = jVault.toObject();
            vault.dir = QDir(jVaultObj["Directory"].toString());
            vault.display_name = vault.dir.dirName();
            vault.password = jVaultObj["Password"].toString();
            vaults.push_back(vault);
            qDebug() << "[JSON] Vault found";
            qDebug() << "  Display name :" << vault.display_name;
            qDebug() << "  Directory    :" << vault.dir.path();
            qDebug() << "  Password     :" << vault.password;
        }
        file.close();
        qDebug() << "[JSON] Json loaded";
        return vaults;
    }

    void SaveVaultJson(const QVector<Vault>& vaults){
        QJsonObject jObj;
        QJsonArray jVaults;
        for (const auto& vault : vaults){
            QJsonObject jVault;
            jVault["Directory"] = vault.dir.path();
            jVault["Password"] = vault.password;
            jVaults.push_back(jVault);
        }
        jObj["Vaults"] = jVaults;

        QJsonDocument jDoc(jObj);

        QFile file(jsonPath);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)){
            qDebug() << "failed to save file";
            return;
        }
        file.write(jDoc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "[JSON] Json saved";
    }

};






#endif // JSON_H
