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

    Vault* GetVault(int index);


    bool SaveVaultData(Vault* vault);
    bool LoadVaultData(Vault* vault);


    void DetachVault(int index);
    void CreateVault(const QString& dir, const QString& password);

signals:
    void onAttachVault(Vault* pVault);
    void onDetachVault(int index);

private:
    VaultManager();
    void LoadVaultManagerData();
    void SaveVaultManagerData();

    static VaultManager instance;
    QVector<Vault*> vaults;
};

