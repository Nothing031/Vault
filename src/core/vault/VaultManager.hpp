#pragma once

#include <QObject>
#include <QVector>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <memory>

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
    std::shared_ptr<Vault> GetVault(int index);

    void ChangeVaultPath(std::shared_ptr<Vault> vault, const QString& path);
    void CreateVault(const bool& aesEnabled, const QString& dir, const QString& password);
    void DetachVault(std::shared_ptr<Vault> vault);

signals:
    void onVaultDataChanged(std::shared_ptr<Vault> vault);
    void onVaultAdded(std::shared_ptr<Vault> vault);
    void onVaultRemoved(std::shared_ptr<Vault> vault);

private:
    VaultManager();

    static VaultManager instance;
    QVector<std::shared_ptr<Vault>> vaults;
};

