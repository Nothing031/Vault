#pragma once

#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "Vault.hpp"


class VaultManager : public QObject
{
    Q_OBJECT
public:
    ~VaultManager();

    static VaultManager& GetInstance();

    void LoadData();
    void SaveData();

    Vault* GetVault(int index);

    void DetachVault(int index);
    void CreateVault(const bool& aesEnabled, const QString& dir, const QString& password);

signals:
    void onAttachVault(Vault* pVault);
    void onDetachVault(int index);

private:
    VaultManager();

    static VaultManager instance;
    QVector<Vault*> vaults;
};

