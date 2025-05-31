#pragma once

#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <memory>
#include <optional>

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

    void CreateVault(const bool& aesEnabled, const QString& dir, const QString& password);
    void DetachVault(std::shared_ptr<Vault> vault);

signals:
    void onVaultAdded(std::shared_ptr<Vault> vault);
    void onVaultRemoved(std::shared_ptr<Vault> vault);

private:
    VaultManager();

    static VaultManager instance;
    QVector<std::shared_ptr<Vault>> vaults;
};

