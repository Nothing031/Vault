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

public slots:
    Vault* GetVault(int index);

    void CreateVault(const bool& aesEnabled, const QString& dir, const QString& password);
    void DetachVault(Vault *vault);

signals:
    void onVaultAdded(Vault* pVault);
    void onVaultRemoved(Vault* vault);

private:
    VaultManager();

    static VaultManager instance;
    QVector<Vault*> vaults;
};

